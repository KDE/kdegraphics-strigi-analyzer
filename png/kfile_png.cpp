#include <stdlib.h>
#include "kfile_png.h"
#include <kurl.h>
#include <kprocess.h>
#include <klocale.h>
#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <kgenericfactory.h>
#include <qdict.h>
#include <qvalidator.h>
#include <kdebug.h>

// this is not to stay here. It's just for testing until the real 
// api is there
static QStringList preferredItems;


// some defines to make it easier
// don't tell me anything about preprocessor usage :)
#define CHUNK_SIZE(data, index) ((data[index  ]<<24) + (data[index+1]<<16) + \
                                 (data[index+2]<< 8) +  data[index+3])
#define CHUNK_TYPE(data, index)  &data[index+4]
#define CHUNK_HEADER_SIZE 12
#define CHUNK_DATA(data, index, offset) data[8+index+offset]

// known translations for common png keys
static const char* knownTranslations[] = {
  I18N_NOOP("Title"),
  I18N_NOOP("Author"),
  I18N_NOOP("Description"),
  I18N_NOOP("Copyright"),
  I18N_NOOP("Creation Time"),
  I18N_NOOP("Software"),
  I18N_NOOP("Disclaimer"),
  I18N_NOOP("Warning"),
  I18N_NOOP("Source"),
  I18N_NOOP("Comment")
};    

// and for the colors
static const char* colors[] = {
  I18N_NOOP("Grayscale"),
  I18N_NOOP("Unknown"),
  I18N_NOOP("RGB"),
  I18N_NOOP("Palette"),
  I18N_NOOP("Grayscale/Alpha"),
  I18N_NOOP("RGB/Alpha")
};

  // and compressions  
char* compressions[] = 
{
  I18N_NOOP("deflate")
};


K_EXPORT_COMPONENT_FACTORY(kfile_png, KGenericFactory<KPngPlugin>("kfile_png"));

KPngPlugin::KPngPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
    ::preferredItems = preferredItems;
}

KFileMetaInfo* KPngPlugin::createInfo(const QString& path)
{
    return new KPngMetaInfo(path);
}


KPngMetaInfo::KPngMetaInfo( const QString& path )
    : KFileMetaInfo(path)
{
    QFile f(path);
    f.open(IO_ReadOnly);
  
    unsigned char *data = (unsigned char*) malloc(f.size()+1);
    f.readBlock((char*)data, f.size());
    data[f.size()]='\n';
  
    // find the start
    if ((data[0] == 137) && (data[1] == 80) && (data[2] == 78) && (data[3] == 71)
     && (data[4] ==  13) && (data[5] == 10) && (data[6] == 26) && (data[7] == 10))
    {
    // ok
        // the IHDR chunk should be the first
        if (!strncmp((char*)&data[12], "IHDR", 4))
      {
          // we found it
          unsigned long x,y;
          x = (data[16] << 24) + (data[17] << 16) + (data[18] << 8) + data[19];
          y = (data[20] << 24) + (data[21] << 16) + (data[22] << 8) + data[23];
      
          int type = data[25];
          int bpp = data[24];
          
          qDebug("resolution %dx%d", x, y);
          
          switch (type)
          {
              case 0: break;           // Grayscale
              case 2: bpp *= 3; break; // RGB
              case 3: break;           // palette
              case 4: bpp *= 2; break; // grayscale w. alpha
              case 5: bpp *= 4; break; // RGBA

              default: // we don't get any sensible value here
                  bpp = 0;
          }
    
          m_items.insert("Resolution", new KFileMetaInfoItem("Resolution",
                         i18n("Resolution"),
                         QVariant(QString("%1 x %2").arg(x).arg(y)), false,
                         QString::null, i18n("pixels")));
      
          m_items.insert("Bitdepth", new KFileMetaInfoItem("Bitdepth",
                         i18n("Bitdepth"), QVariant(bpp), false,
                         QString::null, i18n("bpp")));
      
          m_items.insert("Color mode", new KFileMetaInfoItem("Color mode",
                         i18n("Color mode"), QVariant(
                         i18n((type < sizeof(colors)/sizeof(colors[0])) ? 
                         colors[data[25]] : "Unknown"))));
      
          m_items.insert("Compression", new KFileMetaInfoItem("Compression",
                         i18n("Compression"), QVariant(
                         i18n((data[26]<sizeof(compressions)/sizeof(compressions[0]) ?
                              compressions[data[26]] : "Unknown")))));
      }

    // look for a tEXt chunk
    int index = 8;
    index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
  
    while(index<f.size()-12) {
      while (strncmp((char*)CHUNK_TYPE(data,index), "tEXt", 4)) {
        if (!strncmp((char*)CHUNK_TYPE(data,index), "IEND", 4)) {
          free(data);
          return;
        }
        index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
      }
    
      if (index<f.size()) {
        // we found a tEXt field
        qDebug("We found a tEXt field");
        // get the key, it's a null terminated string at the chunk start
        unsigned char* key = &CHUNK_DATA(data,index,0);

        int keysize = strlen((char*)key);

        // the text comes after the key, but isn't null terminated
        unsigned char* text = &CHUNK_DATA(data,index, keysize+1);
        int textsize = CHUNK_SIZE(data, index)-keysize-1;
        QByteArray arr(textsize);
        arr = QByteArray(textsize).duplicate((const char*)text, textsize);
      
        m_items.insert(QString((char*)key), new KFileMetaInfoItem((char*)key,
                         i18n((char*)key),
                         QVariant(QString(arr)), true));
      
        kdDebug() << "adding " << (char*)key << " / " << QString(arr) << endl;
        
        index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
      } 
    }
  }
  free(data);
}

KPngMetaInfo::~KPngMetaInfo()
{
}

KFileMetaInfoItem * KPngMetaInfo::item( const QString& key ) const
{
    return m_items[key];
}

QStringList KPngMetaInfo::supportedKeys() const
{
    QDictIterator<KFileMetaInfoItem> it(m_items);
    QStringList list;
    
    for (; it.current(); ++it)
    {
        list.append(it.current()->key());
    }
    return list;
}

QStringList KPngMetaInfo::preferredKeys() const
{
    QDictIterator<KFileMetaInfoItem> it(m_items);
    QStringList list;
    
    for (; it.current(); ++it)
    {
        list.append(it.current()->key());
    }
    
    // now move them up
    QStringList::Iterator all;
    QStringList::Iterator pref;
    
    for (pref=::preferredItems.end(); pref!=::preferredItems.begin(); --pref)
    {
        all = list.find(*pref);
        if (all != list.end())
        {
            QString tmp = *all;
            list.remove(all);
            list.prepend(tmp);
        }
    }

    return list;
}

void KPngMetaInfo::applyChanges()
{
    bool doit = false;
    
    // look up if we need to write to the file
    QDictIterator<KFileMetaInfoItem> it(m_items);
    for( ; it.current(); ++it )
    {
        if (it.current()->isModified())
        {
            doit = true;
            break;
        }
    }

    if (!doit) return;

    // todo
}

bool KPngMetaInfo::supportsVariableKeys() const
{
    return true;
}

QValidator * KPngMetaInfo::createValidator( const QString& key, QObject *parent,
                                            const char *name ) const
{
    if (m_items[key]->isEditable())
        return new QRegExpValidator(QRegExp(".*"), parent, name);
    else 
        return 0L;
}

QVariant::Type KPngMetaInfo::type( const QString& key ) const
{
    return m_items[key]->type();
}

#include "kfile_png.moc"

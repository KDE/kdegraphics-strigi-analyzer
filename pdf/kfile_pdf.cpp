#include "kfile_pdf.h"
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
#include <kprocess.h>

K_EXPORT_COMPONENT_FACTORY(kfile_pdf, KGenericFactory<KPdfPlugin>("kfile_pdf"));

KPdfPlugin::KPdfPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
}

KFileMetaInfo* KPdfPlugin::createInfo(const QString& path)
{
    return new KPdfMetaInfo(path);
}

void KPdfMetaInfo::slotReceivedStdout(KProcess*, char* buffer, int buflen)
{
    qDebug("received stdout from child process");
    // just replace the last \n with a 0
    buffer[buflen-1] = '\0';
    QString s(buffer);
    kdDebug() << s << endl;
    QStringList l = QStringList::split("\n", s);
    

    QStringList::Iterator it = l.begin();
    for (; it != l.end(); ++it ) {
        kdDebug() << *it << endl;

        if ((*it).startsWith("CreationDate"))
        {
            m_items.insert("Created", new KFileMetaInfoItem("Created",
                           i18n("Created"),
                           QVariant((*it).mid(13).stripWhiteSpace())));
        }
        else if ((*it).startsWith("ModDate"))
        {
            m_items.insert("Modified", new KFileMetaInfoItem("Modified",
                           i18n("Modified"),
                           QVariant((*it).mid(8).stripWhiteSpace())));
        }
        else if ((*it).startsWith("Pages:"))
        {
            m_items.insert("Pages", new KFileMetaInfoItem("Pages",
                           i18n("Pages"),
                           QVariant((*it).mid(7).stripWhiteSpace().toInt())));
        }
        else if ((*it).startsWith("Encrypted:"))
        {
            bool b = ((*it).mid(10).stripWhiteSpace() == "yes") ? true : false;
            m_items.insert("Encrypted", new KFileMetaInfoItem("Encrypted",
                           i18n("Encrypted"),
                           QVariant(b, 42)));
        }
        else
        {
            QString key( (*it).left( (*it).find(":") ) );
            QString value( (*it).mid((*it).find(":")+1).stripWhiteSpace() );

            m_items.insert(key, new KFileMetaInfoItem(key,
                       i18n(key.latin1()),
                       QVariant(value)));
        }
    }
    
}

KPdfMetaInfo::KPdfMetaInfo( const QString& path )
    : KFileMetaInfo::KFileMetaInfo(path)
{

    KProcess p;
    p << "pdfinfo" << path;
    
    QObject::connect(&p, SIGNAL(receivedStdout(KProcess*, char*, int)),
                     this, SLOT(slotReceivedStdout(KProcess*, char*, int)));

    p.start(KProcess::Block, KProcess::Stdout);
    qDebug("subprocess finished");
}  

    
#if 0
    QFile f(path);
    f.open(IO_ReadOnly);
  
//    unsigned char data[f.size()];
//    f.readBlock((char*)data, f.size());
    QString s;
    f.readLine(s, 9);
    if (!s.startsWith("%PDF-")) // it's not a sane pdf file
    {
        return;
    }
    
    m_items.insert("Version", new KFileMetaInfoItem("Version",
                   i18n("PDF Verison"),
                   QVariant(QString(s.right(3))), false, "V"));
    
    
    // now we have to read backwards from the end of file to find the position
    // of the info object we are interested in
    int pos = f.size()-256;
    int tpos = -1;
    while ((tpos<0)&&(pos>0))
    {
        char data[257];
        f.at(pos);
        f.readBlock(data, 256);
        data[256]='\0';
        QString s(data);
        tpos = s.find("trailer");
        pos-=256;
    }
    
    if (tpos<0) return;
    
    tpos+=pos; // that should be the start position of the trailer in the file
      
    // we are buffered, right? Then we can read it again
    f.at(tpos);
    
    // reed all the items into a qdict
    s = "";
    
    while(!f.atEnd())
    {
        QString tmp;
        f.readLine(tmp, 10000);
        kdDebug() << s;
        s+=tmp;
    }
  }
#endif

KPdfMetaInfo::~KPdfMetaInfo()
{
}

KFileMetaInfoItem * KPdfMetaInfo::item( const QString& key ) const
{
    return m_items[key];
}

QStringList KPdfMetaInfo::supportedKeys() const
{
    QDictIterator<KFileMetaInfoItem> it(m_items);
    QStringList list;
    
    for (; it.current(); ++it)
    {
        list.append(it.current()->key());
    }
    return list;
}

QStringList KPdfMetaInfo::preferredKeys() const
{
    return supportedKeys();
}

void KPdfMetaInfo::applyChanges()
{
}

QVariant::Type KPdfMetaInfo::type( const QString& key ) const
{
    return m_items[key]->type();
}

#include "kfile_pdf.moc"

#ifndef __KFILE_PNG_H__
#define __KFILE_PNG_H__

#include <kfilemetainfo.h>
#include <kurl.h>

class QString;

class KPngMetaInfo: public KFileMetaInfo
{
public:
    KPngMetaInfo( const QString& path );
    virtual ~KPngMetaInfo();
    
    virtual KFileMetaInfoItem * item( const QString& key ) const;
    
    virtual QStringList supportedKeys() const;
    virtual QStringList preferredKeys() const;
    virtual bool supportsVariableKeys() const;
    
    virtual void applyChanges();
    virtual QValidator * createValidator( const QString& key, QObject *parent,
                                          const char *name ) const;

    QVariant::Type type( const QString& key ) const;  
        
private:
    char* m_path;
};

class KPngPlugin: public KFilePlugin
{
    Q_OBJECT
    
public:
    KPngPlugin( QObject *parent, const char *name,
                const QStringList& preferredItems );
    
    virtual KFileMetaInfo* createInfo( const QString& path );
};


#endif

#ifndef __KFILE_PDF_H__
#define __KFILE_PDF_H__

#include <kfilemetainfo.h>
#include <kurl.h>
#include <qobject.h>

class QString;
class KProcess;

class KPdfMetaInfo: public QObject, public KFileMetaInfo
{
    // we have a slot, so we need to be a QObject
    Q_OBJECT
public:
    KPdfMetaInfo( const QString& path );
    virtual ~KPdfMetaInfo();
    
    virtual KFileMetaInfoItem * item( const QString& key ) const;
    
    virtual QStringList supportedKeys() const;
    virtual QStringList preferredKeys() const;
    
    virtual void applyChanges();

    QVariant::Type type( const QString& key ) const;
    

private slots:
    void slotReceivedStdout(KProcess* p, char* buffer, int buflen);
};

class KPdfPlugin: public KFilePlugin
{
    Q_OBJECT
    
public:
    KPdfPlugin( QObject *parent, const char *name,
                const QStringList& preferredItems );
    
    virtual KFileMetaInfo* createInfo( const QString& path );
};


#endif

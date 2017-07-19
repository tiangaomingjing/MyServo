#ifndef FACTORY_H
#define FACTORY_H
#include <QWidget>

class Factory
{
public:
  Factory();
public:
    template<typename T>
    static void registerClass()
    {
        constructors().insert( T::staticMetaObject.className(), &constructorHelper<T> );
    }

    static QWidget* createObject( const QByteArray& className, QWidget* parent = NULL )
    {
        Constructor constructor = constructors().value( className );
        if ( constructor == NULL )
            return NULL;
        return (*constructor)( parent );
    }

private:
    typedef QWidget* (*Constructor)( QWidget* parent );

    template<typename T>
    static QWidget* constructorHelper( QWidget* parent )
    {
        return new T( parent );
    }

    static QHash<QByteArray, Constructor>& constructors()
    {
        static QHash<QByteArray, Constructor> instance;
        return instance;
    }
};

#endif // FACTORY_H

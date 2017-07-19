#------------set the output dir-------------
#PWD表示当前这个文件的路径
#..表示再往上一个目录。
#OUT_PWD表示makefile的目录

INCLUDEPATH += $${PWD}/..\
               $${PWD}/../XmlBuilder\
               $${PWD}/../ServoControl\
               $${PWD}/../NetCmd/ServoDriverComDll\
               $${PWD}/../NetCmd/ServoDriverComDll/NetCom/include\
               $${PWD}/AbstractFuncWidget/QmlFactory


CONFIG(debug, debug|release){
    FUNC_OUT_ROOT = $${PWD}/../../debug/Bin
    FUNC_LIB_ROOT=$${FUNC_OUT_ROOT}
    LIBS +=$${FUNC_LIB_ROOT}/ServoDriverComDlld.lib
} else{
    FUNC_OUT_ROOT = $${PWD}/../../release/Bin
    FUNC_LIB_ROOT=$${FUNC_OUT_ROOT}
    LIBS += $${FUNC_LIB_ROOT}/ServoDriverComDll.lib
}
DESTDIR =$${FUNC_OUT_ROOT}



#include "ULuaInterpreter.h"

#include <string>

#include <QDropEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include "../UniCore/ULua.h"
#include "../UniCore/ULog.h"

using namespace std;

namespace uni
{

QMap<lua_State *,ULuaInterpreter *> ULuaInterpreter::interpreters_;

ULuaInterpreter::ULuaInterpreter(lua_State *L /*= 0*/, QWidget *parent /*= 0*/)
:QWidget(parent)
,L_(L)
{
    if(!L_)
    {
        L_ = lua_open();
        if(!L_)
        {
            UERROR("Lua解释器")<<"无法创建Lua状态，内存不足。";
            return;
        }
        lua_gc(L_,LUA_GCSTOP,0);
        luaL_openlibs(L_);
        RegisterCommonLuaFunctions(L_);
        registerPrint();  //修改print函数为显示到output编辑框。
        lua_gc(L_,LUA_GCRESTART,0);
    }

    //将L_和当前窗体记录到interpreters_中。
    interpreters_[L_] = this;

    scriptEdit_ = new QTextEdit(this);
    outputEdit_ = new QTextEdit(this);
    execButton_ = new QPushButton(tr("&exec"),this);
    stopButton_ = new QPushButton(tr("&stop"),this);
    stopButton_->setEnabled(false);

    QHBoxLayout *layout = new QHBoxLayout;
    QSplitter *splitter = new QSplitter(Qt::Vertical,this);
    splitter->addWidget(scriptEdit_);
    splitter->addWidget(outputEdit_);
    splitter->setStretchFactor(0,3);
    splitter->setStretchFactor(1,1);
    layout->addWidget(splitter);
    splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout *layout2 = new QVBoxLayout;
    
    layout2->addWidget(execButton_);
    layout2->addWidget(stopButton_);
    layout->addLayout(layout2);
    setLayout(layout);

    execRoutine_ = new ULuaInterpreter_ExecRoutine(this);

    setAcceptDrops(true);

    connect(execButton_,SIGNAL(clicked()),this,SLOT(execScript()));
    connect(stopButton_,SIGNAL(clicked()),this,SLOT(stopScript()));
    connect(execRoutine_,SIGNAL(output(const QString &)),this,SLOT(output(const QString &)));
    connect(execRoutine_,SIGNAL(started()),this,SLOT(scriptStarted()));
    connect(execRoutine_,SIGNAL(finished()),this,SLOT(scriptStopped()));
}

ULuaInterpreter::~ULuaInterpreter()
{
    interpreters_.remove(L_);
    lua_close(L_);
}

void ULuaInterpreter::registerLuaFunctions( lua_CFunction registerFunction )
{
    registerFunction(L_);
}

static int traceback (lua_State *L) {
    if (!lua_isstring(L, 1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);  /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);  /* call debug.traceback */
    return 1;
}

static void lstop(lua_State *L,lua_Debug *ar)
{
    lua_sethook(L,NULL,0,0);
    luaL_error(L,"interrupted!");
}

void ULuaInterpreter::execScript()
{
    outputEdit_->clear();
    execRoutine_->execScript(L_,scriptEdit_->toPlainText());
}

void ULuaInterpreter::registerPrint()
{
    lua_pushcfunction(L_,ULuaInterpreter::lua_print);
    lua_setglobal(L_,"print");
}

int ULuaInterpreter::lua_print( lua_State *L )
{
    QString result;
    for(int i = 1; i <= lua_gettop(L); i++)
    {
        switch(lua_type(L,i))
        {
        case LUA_TNIL:
            {
                result+="nil ";
                break;
            }
        case LUA_TBOOLEAN:
            {
                if(lua_toboolean(L,i) == 0)
                {
                    result+="false ";
                }
                else if(lua_toboolean(L,i) == 1)
                {
                    result += "true ";
                }
                else
                {
                    assert(!"lua_toboolean返回值不为0也不为1。");
                }
                break;
            }
        case LUA_TLIGHTUSERDATA:
            {
                void *userData = lua_touserdata(L,i);
                result += QString("light userdata:%1 ").arg((unsigned int)userData,8,16,QChar('0'));
                break;
            }
        case LUA_TNUMBER:
        case LUA_TSTRING:
            {
                result += QString("%1 ").arg(lua_tostring(L,i));
                break;
            }
        case LUA_TTABLE:
            {
                const void *tablePtr = lua_topointer(L,i);
                result += QString("table:%1 ").arg((unsigned int)tablePtr,8,16,QChar('0'));
                break;
            }
        case LUA_TFUNCTION:
            {
                const void *function = lua_topointer(L,i);
                result += QString("function:%1 ").arg((unsigned int)function,8,16,QChar('0'));
                break;
            }
        case LUA_TUSERDATA:
            {
                void *userData = lua_touserdata(L,i);
                result += QString("userdata:%1 ").arg((unsigned int)userData,8,16,QChar('0'));
                break;
            }
        case LUA_TTHREAD:
            {
                const void *threadPtr = lua_topointer(L,i);
                result += QString("thread:%1 ").arg((unsigned int)threadPtr,8,16,QChar('0'));
                break;
            }
        default:
            {
                assert(!"未知的lua类型。");
            }
        }

    }
    bool ok = QMetaObject::invokeMethod(interpreters_[L],"output",Qt::BlockingQueuedConnection,Q_ARG(QString,result));
    assert(ok);
    return 0;
}

void ULuaInterpreter::output( const QString &msg )
{
    outputEdit_->append(msg);
}

void ULuaInterpreter::stopScript()
{
    lua_sethook(L_,lstop,LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

void ULuaInterpreter::scriptStarted()
{
    execButton_->setEnabled(false);
    stopButton_->setEnabled(true);
}

void ULuaInterpreter::scriptStopped()
{
    stopButton_->setEnabled(false);
    execButton_->setEnabled(true);
}

void ULuaInterpreter::dropEvent( QDropEvent *event )
{
    foreach(QString format,event->mimeData()->formats())
    {
        UTRACE<<format.toStdString();
    }
    wstring path = (wchar_t *)event->mimeData()->data("FileNameW").data();
    QFile file(QString::fromStdWString(path));
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        UERROR("Lua解释器")<<"无法打开文件。";
        return;
    }
    int size = file.size();
    QByteArray content = file.read(size);
    if(size >= 3 && content[0] == '\xEF'
        && content[1] == '\xBB' && content[2] == '\xBF')
    {
        //utf-8编码。
        //content.remove(0,3);
        scriptEdit_->setPlainText(QString::fromUtf8(content,content.size()));
    }
    else if(size >= 2 && content[0] == '\xFF' && content[1] == '\xFE')
    {
        scriptEdit_->setPlainText(QString::fromUtf16((unsigned short *)content.data()));
    }
    else if(size >= 2 && content[0] == '\xFE' && content[1] == '\xFF')
    {
        scriptEdit_->setPlainText(QString::fromUtf16((unsigned short *)content.data()));
    }
    else
    {
        scriptEdit_->setPlainText(QString::fromLocal8Bit(content));
    }
    event->acceptProposedAction();
}

void ULuaInterpreter::dragEnterEvent( QDragEnterEvent *event )
{
    if(event->mimeData()->hasFormat("FileNameW"))
    {
        event->acceptProposedAction();
    }
}

void ULuaInterpreter_ExecRoutine::run()
{
    int result = luaL_loadstring(L_,script_.toLocal8Bit());
    if(result == LUA_ERRSYNTAX)
    {
        emit output(tr("Syntax Error:%1").arg(QString::fromLocal8Bit(luaL_checkstring(L_,-1))));
        return;
    }
    else if(result == LUA_ERRMEM)
    {
        emit output(tr("Memory Error:%1").arg(QString::fromLocal8Bit(luaL_checkstring(L_,-1))));
        return;
    }

    assert(result == 0);

    lua_pushcfunction(L_,traceback);
    lua_insert(L_,1);
    result = lua_pcall(L_,0,0,1);
    lua_remove(L_,1);
    if(result == LUA_ERRRUN)
    {
        emit output(tr("Runtime Error:%1").arg(QString::fromLocal8Bit(luaL_checkstring(L_,-1))));

        return;
    }
    else if(result == LUA_ERRMEM)
    {
        emit output(tr("Memory Error:%1").arg(QString::fromLocal8Bit(luaL_checkstring(L_,-1))));
        return;
    }
    else if(result == LUA_ERRERR)
    {
        emit output(tr("Error Error:%1").arg(QString::fromLocal8Bit(luaL_checkstring(L_,-1))));
        return;
    }

    assert(result == 0);
}

ULuaInterpreter_ExecRoutine::ULuaInterpreter_ExecRoutine( QObject *parent /*= 0*/ )
:QThread(parent)
{

}

ULuaInterpreter_ExecRoutine::~ULuaInterpreter_ExecRoutine()
{
    wait();
}

void ULuaInterpreter_ExecRoutine::execScript( lua_State *L,const QString &script )
{
    L_ = L;
    script_ = script;

    if(!isRunning())
    {
        start(LowPriority);
    }
}

}//namespace uni
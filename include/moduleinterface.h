#ifndef MODULEINTERFACE
#define MODULEINTERFACE

class IMainFrame;

//模块接口，所有需要被框架加载的项目都要实现此接口
//接口中所有const char*类型均为UTF8编码。
class IModule
{
public:
    IModule() {}

    virtual ~IModule() {}

    //定义返回的窗口的类型
    enum eMainWindowType
    {
        MT_QWIDGET,     //QWidget类型的指针
        MT_HWND,        //HWND
    };

    //模块被加载后由框架调用
    virtual void InitInstance() = 0;

    //模块被析构前由框架调用
    virtual void UnInitInstance() = 0;

    /**
     * @brief       界面加载完成后由框架调用，并向模块传递框架接口的指针
     * @param[in]	pMainFrame 框架接口的指针
     */
    virtual void UiInitComplete(IMainFrame* pMainFrame) {}

    /**
     * @brief       获取模块的主窗口
     * @details     当切换到该模块的标签时，此窗口会被显示到MainFrame中。可以返回空指针。
     * @return      根据GetMainWindowType的返回值返回主模块主窗口的QWidget对象的指针或HWND句柄
     */
    virtual void* GetMainWindow() = 0;

    //获取返回的主窗口的类型
    virtual eMainWindowType GetMainWindowType() const = 0;

    //获取模块名称
    virtual const char* GetModuleName() = 0;

    /**
     * @brief       当主窗口触发了一个命令时由框架调用
     * @param[in]	strCmd：命令的ID
     * @param[in]	checked：命令按钮是否被选中
     */
    virtual void OnCommand(const char* strCmd, bool checked) = 0;

    /**
    * @brief       响应Ribbon标签中Combox或ListWidget当前选中项改变，或LineEdit或TextEdit文本改变
    * @param[in]   strId: 命令的ID
    * @param[in]   index: 选中项的序号
    * @param[in]   text: 选中项的文本
    */
    virtual void OnItemChanged(const char* strId, int index, const char* text) {}

    /**
     * @brief       向模块发送一个消息，用于在同一个进程之间的各个模块发送消息
     * @param[in]	msgType 消息类型
     * @param[in]	msgData 消息体
     * @return      返回的数据（如果需要的话）
     */
    virtual void* OnMessage(const char* msgType, void* para1 = nullptr, void* para2 = nullptr) {  return nullptr; }

    //当主窗口中切换到模块的标签页时响应
    virtual void OnTabEntered() {}

    /**
     * @brief       程序在退出前由框架调用
     */
    virtual void OnAppExit() {}

    /**
     * @brief       创建一个用户自定义控件（仅Qt模块支持此接口）
     * @note        当xml中使用了UserWidget节点时此函数会被框架调用以创建自定义控件，模块中根据id创建对应的控件，并返回其指针。
     * @param[in]	strId 控件的id
     * @param[in]	pParent 控件的父窗口（QWidget指针）
     * @return      创建的控件（QWidget指针）
     */
    virtual void* CreateUserWidget(const char* strId, void* pParent = nullptr) {  return nullptr; }

};

//一个形如IModule* func()的函数的指针
//模块实现IModule接口后，还应该导出一个名为“CreateInstance”的函数以创建对象，创建的对象由框架负责释放
#if defined(WIN32) || defined(WIN64)
typedef IModule* (__stdcall *pfCreateModuleInstance)();
#else
typedef IModule* (*pfCreateModuleInstance)();
#endif

#endif // MODULEINTERFACE


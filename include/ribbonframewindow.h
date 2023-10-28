#ifndef RIBBONFRAMEWINDOW_H
#define RIBBONFRAMEWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QString>
#include "moduleinterface.h"
#include "mainframeinterface.h"
#include "mainframe_global.h"

class QDomElement;
class QToolBar;
class QToolButton;
class QVBoxLayout;
class MainFramePrivate;

class RIBBONFRAME_EXPORT RibbonFrameWindow : public QMainWindow, public IMainFrame
{
    Q_OBJECT

public:
    /**
     * @brief       构造函数
     * @param[in]	parent 父窗口
     * @param[in]	xmlPath xml文件的路径，如果为空，则使用可执行文件相同目录下的“MainFrame.xml”
     * @param[in]	initUiManual 是否手动初始化UI，如果为false中，则在构造函数中完成UI的初始化，否则，构造函数不初始化UI，需要手动调用InitUi函数。
     */
    RibbonFrameWindow(QWidget *parent = nullptr, const QString& xmlPath = QString(), bool initUiManual = false);
    virtual ~RibbonFrameWindow();

    //UI初始化函数，包含了xml文件的解析、模块的加载等操作。
    //默认情况下会在构造函数中被调用，如果构造函数的参数initUiManual为true，则需要手动调用此函数。
    void InitUi();

signals:

private slots:
    void OnTabIndexChanged(int index);      //响应标签切换
    void OnTabBarClicked(int index);
    void OnTabBarDoubleClicked(int index);
    void OnActionTriggerd(bool checked);    //响应任意一个工具栏中的按钮被点击
    void OnItemIndexChanged(int index);     //响应工具栏中ComboBox或ListWidget当前项改变
    void OnEditTextChanged(const QString& text);    //响应LineEdit文本改变
    void OnEditTextChanged();    //响应TextEdit文本改变
    void FocusChanged(QWidget* old, QWidget* now);

private:
    void LoadUIFromXml(QString xmlPath);           //从xml文件加载界面
    IModule* LoadPlugin(const QString &strModulePath);  //加载一个模块
    void LoadMainFrameUi(const QDomElement& element);   //从一个xml节点加载界面
    void LoadUiElement(const QDomElement& element, QToolBar* pToolbar);     //加载一组UI元素（用于Ribbin的Page）
    void LoadSimpleToolbar(const QDomElement& element, QToolBar* pToolbar);   //加载一组Action元素，图标全部为小图标（用于快速启动栏）

    QAction* LoadUiAction(const QDomElement& element);    //从一个xml节点加载Action
    QWidget* LoadUiWidget(const QDomElement& element, QWidget* pParent, bool& smallIcon); //从一个xml节点加载Widget
    QMenu* LoadUiMenu(const QDomElement& element, bool enableWidget = true);      //从一个xml节点加载菜单
    void LoadMenuItemFromXml(const QDomElement& element, QMenu* pMenu, bool enableWidget);

    /**
     * @brief       向界面添加一个控件
     * @param[in]	pUiWidget 添加的控件
     * @param[in]	smallIcon 是否为小图标
     * @param[in]	pToolbar 要添加控件的工具栏
     * @param[in][out]	previousLayout 当控件为小图标时垂直排列的布局
     */
    void AddUiWidget(QWidget* pUiWidget, bool smallIcon, QToolBar* pToolbar, QVBoxLayout*& previousLayout);       //向工具栏界面添加一个控件

    void InitMenuButton(QToolButton* pMenuBtn, const QDomElement& element);     //根据一个xml节点设置菜单按钮的状态

    void ApplyRadioButtonGroup();

    QWidget* GetModuleMainWindow(IModule* pModule);  //获取模块的主窗口

    void SetRibbonPin(bool pin);
    void ShowHideRibbon(bool show);

    QAction* AddRibbonContextAction(const QString& strId, const QString& strName);

protected:
    //获取当前模块
    IModule* CurrentModule() const;

    /**
     * @brief       响应一个命令。
     * @note        当点击工具栏中的Action、CheckBox或RadioButton时会触发此函数。重写此函数用于响应需要在主窗口中响应的命令，
     *              需要在模块中响应的命令请在对应模块的OnCommand函数中响应。如果派生类重写了此函数，请在重写的函数中调用基类的函数。
     * @param[in]	strCmd：命令的ID
     * @param[in]	checked：命令是否选中（如果命令具有可选中属性时）
     * @return      返回true表示命令已经被响应。如果命令需要在模块中响应，请返回false。
     */
    virtual bool OnCommand(const QString& strCmd, bool checked);

    /**
    * @brief       响应Ribbon标签中Combox或ListWidget当前选中项改变，或LineEdit或TextEdit文本改变
    * @param[in]   strId: 命令的ID
    * @param[in]   index: 选中项的序号
    * @param[in]   text: 选中项的文本
    */
    virtual void OnItemChanged(const QString& strId, int index, const QString& text) {}

    QAction *_GetAction(const QString& strCmd) const;
    QWidget *_GetWidget(const QString& strCmd) const;
    QMenu *_GetMenu(const QString& strCmd) const;
    void SetItemIcon(const QString& strId, const QIcon& icon);

    void SetTabIndex(int index);
    int GetTabIndex() const;

    /**
     * @brief       设置默认主窗口
     * @note        此函数用于设置一个默认的窗口，当MainFrame.xml中Page节点未配置modulePath属性时，切换到该标签时将显示此窗口
     * @param[in]	pWidget：要设置的窗口
     */
    void SetDefaultWidget(QWidget* pWidget);

    /**
     * @brief       创建一个用户自定义控件
     * @note        当xml中使用了UserWidget节点时此函数会被框架调用以创建自定义控件，模块中根据id创建对应的控件，并返回其指针。
     *              注意：默认情况下此函数是在构造函数中调用的，因此如果派生类中重写了此函数，那么重写的函数将不会被调用。要解决这个问题，
     *              需要将构造函数中的initUiManual设置为true，然后在派生类的构造函数中调用InitUi。
     * @param[in]	strId 控件的id
     * @param[in]	pParent 控件的父窗口
     * @return      创建的控件
     */
    virtual QWidget* CreateUserWidget(const QString& strId, QWidget* pParent = nullptr) { return nullptr; }

private:
    MainFramePrivate* d;        //私有成员变量，定义在cpp文件中

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent*) override;

    // IMainFrame interface
public:
    virtual IModule *GetModule(const char* strModuleName) const override;
    virtual void SetItemEnable(const char* strCmd, bool bEnable) override;
    virtual bool IsItemEnable(const char* strCmd) override;
    virtual void SetItemChecked(const char* strCmd, bool checked) override;
    virtual bool IsItemChecked(const char* strCmd) override;
    virtual void* SendModuleMessage(const char* moduleName, const char* msgType, void* para1 = nullptr, void* para2 = nullptr) override;
    virtual const char* GetItemText(const char* strId) override;
    virtual void SetItemText(const char* strId, const char* text) override;
    virtual void SetItemIcon(const char* strId, const char* iconPath, int iconSize) override;
    virtual void* GetAcion(const char* strId) override;
    virtual void* GetWidget(const char* strId) override;
    virtual void* GetMenu(const char* strId) override;
    virtual void SetStatusBarText(const char* text, int timeOut) override;
    virtual int GetItemCurIndex(const char* strId) override;
    virtual void SetItemCurIIndex(const char* strId, int index) override;
};

#endif // RIBBONFRAMEWINDOW_H

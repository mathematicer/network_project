// ACMNetDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "NetSet.h"
#include "ACMNet.h"
#include "ACMNetDlg.h"
#include "ACMBrow.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum  STATE  { THREAD_EXIT, THREAD_WAIT, THREAD_RUN }  FLAG;
CEdit    *CtrlLog;
CACMNetDlg *mynet;
CString  cmd, result;
ACMBrow myweb;
HtmlMsg  myhtml;

void PrintLog(LPCTSTR str)
{
	CString log;
	log.Format(_T("%s\r\n"), str);
	CtrlLog->ReplaceSel(log);
}
void PrintRP(LPCTSTR txt){     //��txt�����Response�༭��
	CString res;
	res.Format(_T("Response: %s\r\n"), txt);
	((CEdit *)(mynet->GetDlgItem(IDC_RESPONSE)))->ReplaceSel(res);
}

void  GetPage(CString url){
	mynet->GetDlgItem(IDC_RESPONSE)->ShowWindow(SW_HIDE);
	COleVariant noArg;
	myweb.Navigate(url, &noArg, &noArg, &noArg, &noArg);
	myweb.get_Visible();
}

void GetHtml(CString url){
	if (myweb.IsWindowVisible()){
		myweb.put_Visible(FALSE);
		mynet->GetDlgItem(IDC_RESPONSE)->ShowWindow(SW_SHOW);
	}
	myhtml.SetUrl(url);
	if (!myhtml.HtmlAsk()){
		//AfxMessageBox(myhtml.GetLog());
		PrintLog(myhtml.GetLog());
		return;
	}
	mynet->GetDlgItem(IDC_RESPONSE)->SetWindowText(myhtml.GetText());
}
void Compute(CString equation){
	CString rst;
	int a, b;
	typedef int(*ADDPROC)(int, int);
	mynet->dlltest.Set_fun_name("add");
	ADDPROC Add = (ADDPROC)mynet->dlltest.Getfun();
	int pos = equation.Find(_T("+"));
	a = _ttoi(equation.Mid(0, pos));
	b = _ttoi(equation.Mid(pos + 1));
	rst.Format(_T("%d + %d = %d"), a, b, Add(a, b));
	PrintRP(rst);
}
void CleanRP()
{
	mynet->GetDlgItem(IDC_RESPONSE)->SetWindowText(_T(""));
}
void Switch(){
	if (mynet->GetDlgItem(IDC_RESPONSE)->IsWindowVisible()){
		mynet->GetDlgItem(IDC_RESPONSE)->ShowWindow(SW_HIDE);
		myweb.put_Visible(TRUE);
	}
	else {
		myweb.put_Visible(FALSE);
		mynet->GetDlgItem(IDC_RESPONSE)->ShowWindow(SW_SHOW);
	}
}

void SetDll(CString args){
	mynet->dlltest.Setpath(args);
	if (!mynet->dlltest.Load_dll()){
		PrintLog(_T("�������ӿ�ʧ��!"));
		return;
	}
	PrintLog(_T("�������ӿ�ɹ�!"));
}


void Help_Msg(){
	CString Msg = _T("	help -- look for the help message \r\n \
	getpage -- get the page according to url \r\n \
	gethtml -- get the html according to url \r\n \
	switch -- switch the html and page \r\n \
	setdll -- load the dll from the path \r\n \
      	compute -- compute the num \r\n \
	");
	mynet->GetDlgItem(IDC_RESPONSE)->SetWindowText(Msg);//OutMsg()
}

void MapTask(){
	int len, i;
	TCHAR c;
	CString Ins;
	static CString args;
	args.Empty();
	len = cmd.GetLength();
	for (i = 0; i < cmd.GetLength(); i++){
		if ((c = cmd.GetAt(i)) != _T(' ')){
			Ins += c;
		}
		else break;
	}
	while (i < cmd.GetLength() && (c = cmd.GetAt(i++)) != _T(' '));
	args = cmd.Mid(i);
	if (Ins.Compare(_T("gethtml")) == 0)
		GetHtml(args);
	else if (Ins.Compare(_T("compute")) == 0)
		Compute(args);
	else if (Ins.Compare(_T("getpage")) == 0)
		GetPage(args);
	else if (Ins.Compare(_T("setdll")) == 0)
		SetDll(args);
	else if (Ins.Compare(_T("switch")) == 0)
		Switch();
	else if (Ins.Compare(_T("cleanrp")) == 0)
		CleanRP();
	else if (Ins.Compare(_T("help")) == 0){
		Help_Msg();
	}
	else{
		PrintLog(_T("����δ����"));
	}
}

DWORD WINAPI DEALCMD(LPVOID lpParameter){
	while (TRUE){
		if (FLAG == THREAD_EXIT)
			break;
		else if (FLAG == THREAD_WAIT)
			continue;
		else {
			MapTask();
			FLAG = THREAD_WAIT;
		}

	}
	return 0;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
// �Ի�������
	enum { IDD = IDD_ABOUTBOX };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CACMNetDlg �Ի���



CACMNetDlg::CACMNetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CACMNetDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_CONN);
}

void CACMNetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMAND, mycmd);
}

BEGIN_MESSAGE_MAP(CACMNetDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND, &CACMNetDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CANCEL, &CACMNetDlg::OnBnClickedCancel)
	ON_COMMAND(ID_SETTING, &CACMNetDlg::OnSetting)
	ON_COMMAND(ID_SHOWSET, &CACMNetDlg::OnShowset)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CACMNetDlg ��Ϣ��������

BOOL CACMNetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ����Ӷ���ĳ�ʼ������
	CRect rect;
	mynet = this;
	GetDlgItem(IDC_RESPONSE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	myweb.Create(NULL, NULL, WS_VISIBLE, rect, this, 101);
	myweb.put_Silent(TRUE);//
	CtrlLog = (CEdit *)GetDlgItem(IDC_LOG);
	FLAG = THREAD_WAIT;
	if (CreateThread(NULL, NULL, DEALCMD, NULL, NULL, NULL)){
		PrintLog(_T("������Ϣ�����̳߳ɹ�!\n"));
	}
	else PrintLog(_T("������Ϣ�����߳�ʧ��"));
	mycmd.ReplaceSel(_T("Command: "));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CACMNetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CACMNetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CACMNetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CACMNetDlg::OnClose()
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
	if (AfxMessageBox(_T(" �Ƿ��˳����� \n"), MB_OKCANCEL, MB_ICONQUESTION) == IDOK){
		CRect rectDlg;
		this->GetWindowRect(rectDlg);
		while (rectDlg.TopLeft().y < rectDlg.BottomRight().y){//rectDlg.Height() > 30
			//rectDlg.Height() -= 10;
			rectDlg.TopLeft().y += 3;
			rectDlg.BottomRight().y -= 3;
			this->MoveWindow(rectDlg);
		}
		CDialogEx::OnClose();
	}
	return;

}


void CACMNetDlg::OnBnClickedSend()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	CString strLine, strText;
	int count=mycmd.GetLineCount();
	int len = mycmd.LineLength(mycmd.LineIndex(count-1));
	mycmd.GetLine(count - 1, strText.GetBuffer(len), len);
	strText.ReleaseBuffer(len);
	strLine.Format(_T("%s"), strText);
	mycmd.ReplaceSel(_T("\r\n\r\nCommand: "));
	cmd=strLine.Mid(9);
	if (cmd == _T("")) return;
	mycmd.SetFocus();
	FLAG = THREAD_RUN;
	PrintLog(_T("����ͳɹ�"));
}


void CACMNetDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ����ӿؼ�֪ͨ�����������
	GetDlgItem(IDC_COMMAND)->SetWindowText(_T(""));
}


void CACMNetDlg::OnSetting()
{
	// TODO:  �ڴ�����������������
	NetSet  Dlg;
	Dlg.DoModal();
}


void CACMNetDlg::OnShowset()
{
	// TODO:  �ڴ�����������������
}


int CACMNetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  �ڴ�������ר�õĴ�������

	return 0;
}


HBRUSH CACMNetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{	
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_RESPONSE) //��̬�ı� 
	{
		  pDC->SetBkMode(TRANSPARENT); 
		  HBRUSH B= CreateSolidBrush(RGB(0XFA, 0XFA, 0XD2));
	      pDC->SetTextColor(RGB(255, 0, 0)); //�ؼ��е����ֵ���ɫ
		  return B;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_LOG){
		pDC->SetBkMode(TRANSPARENT); 
		HBRUSH B = CreateSolidBrush(RGB(0x97, 0x79, 0xEE));
		pDC->SetTextColor(RGB(255, 0, 0)); //�ؼ��е����ֵ���ɫ
		return B;
	}
	return hbr;
}
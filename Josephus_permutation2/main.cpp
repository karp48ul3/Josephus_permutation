#include <afxwin.h>  // Core and standard MFC components
#include <vector>
#include <string>
#include <atlbase.h>

#define IDC_MY_BUTTON 1001
#define IDC_MY_BUTTON_RUN_ALL 1007
#define IDC_MY_EDIT_NUMBERS 1002
#define IDC_MY_EDIT_STEP 1003
#define IDC_MY_LABEL_NUMBERS 1004
#define IDC_MY_LABEL_STEP 1005
#define IDC_MY_BUTTON_DRAW_SEQ 1006

std::vector<int> inputNumbers; // stores numbers in range [1,n] where n is specified in text edit 'Num'
int step_size = -1;

int heightOfInputNumbers = 20;

std::vector<int> elimination_order; // stores Josephus permutation

int indexOfEliminationOrder = 0; // stores index of last integer produced when click 'Run once' button

bool runAllPressed = false; // Button 'Run all' was pressed

bool runOncePressed = false; // Button 'Run once' was pressed

std::vector<int> getJosephusSequence(int n, int m);

// 1. Define a custom Main Window class to handle drawing
class CMyWindow : public CFrameWnd
{
private:
    CButton m_MyButton;
    CButton runAllButton;
    CButton buttonDrawSequence;
    CEdit numbers;
    CEdit step;
    CStatic label_numbers;
    CStatic label_step;
public:
    CMyWindow()
    {
        // Create the physical window
        Create(NULL, _T("Josephus Permutation"));
    }

protected:
    afx_msg void OnMyButtonClick(); // button Run once handler
    // button Draw Sequence handler
    afx_msg void OnButtonDrawSeqClick();
    // button Run All handler
    afx_msg void OnButtonRunAll();
    // Declare the paint handler function
    afx_msg void OnPaint();
    // Declare the oncreate handler function
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    // Declare the onSize changed handler function
    afx_msg void OnSize(UINT nType, int cx, int cy);
    // Declare the message map macro
    DECLARE_MESSAGE_MAP()
};

// 2. Map the WM_PAINT message to our OnPaint function
BEGIN_MESSAGE_MAP(CMyWindow, CFrameWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_MY_BUTTON, &CMyWindow::OnMyButtonClick)
    ON_BN_CLICKED(IDC_MY_BUTTON_DRAW_SEQ, &CMyWindow::OnButtonDrawSeqClick)
    ON_BN_CLICKED(IDC_MY_BUTTON_RUN_ALL, &CMyWindow::OnButtonRunAll)
END_MESSAGE_MAP()

int CMyWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    // 1. Call the base class implementation first
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1) {
        return -1; // Fail initialization if base creation fails
    }
    CRect rect;
    GetClientRect(&rect);

    int clientWidth = rect.Width();
    int clientHeight = rect.Height();

    CRect rectButton(clientWidth -150, 20, clientWidth-30 , 50); //

    // Create the button control
    m_MyButton.Create(
        _T("Run once"),                               // Button Text
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // Window & Button Styles
        rectButton,                                   // Layout Rectangle
        this,                                         // Parent Window Pointer
        IDC_MY_BUTTON                                 // Control ID
    );

    CRect rectButton2(clientWidth - 150, 140, clientWidth - 30, 170); //

    runAllButton.Create(
        _T("Run all"),                               // Button Text
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // Window & Button Styles
        rectButton2,                                   // Layout Rectangle
        this,                                         // Parent Window Pointer
        IDC_MY_BUTTON_RUN_ALL                                // Control ID
    );

    CRect rectButtonDrawSeq(clientWidth - 150, 100, clientWidth - 30, 130); //

    // Create the button control
    buttonDrawSequence.Create(
        _T("Draw seq"),                               // Button Text
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // Window & Button Styles
        rectButtonDrawSeq,                                   // Layout Rectangle
        this,                                         // Parent Window Pointer
        IDC_MY_BUTTON_DRAW_SEQ                                 // Control ID
    );

    CRect rectEditNumbers(clientWidth - 210, 60, clientWidth - 130, 90);
    numbers.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, rectEditNumbers, this, IDC_MY_EDIT_NUMBERS);

    CRect rectLabelNumbers(clientWidth - 260, 60, clientWidth - 220,90);
    label_numbers.Create(L"Num", SS_CENTERIMAGE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, rectLabelNumbers, this, IDC_MY_LABEL_NUMBERS);

    CRect rectEditStep(clientWidth - 70, 60, clientWidth - 10, 90);
    step.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, rectEditStep, this, IDC_MY_EDIT_STEP);

    CRect rectLabelStep(clientWidth - 120, 60, clientWidth - 80, 90);
    label_step.Create(L"Step", SS_CENTERIMAGE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, rectLabelStep, this, IDC_MY_LABEL_STEP);


    return 0;
}

void CMyWindow::OnSize(UINT nType, int cx, int cy) {
     CFrameWnd::OnSize(nType, cx, cy);

     CRect rect;
     GetClientRect(&rect);

     int newX = rect.right - 150;
     int newY = rect.top + 20;

     m_MyButton.SetWindowPos(NULL, newX, newY, 120, 30, SWP_NOSIZE | SWP_NOZORDER);
     numbers.SetWindowPos(NULL, rect.right - 210, 60, 80, 30, SWP_NOSIZE | SWP_NOZORDER);
     label_numbers.SetWindowPos(NULL, rect.right - 260, 60, 40, 30, SWP_NOSIZE | SWP_NOZORDER);
     step.SetWindowPos(NULL, rect.right - 70, 60, 60, 30, SWP_NOSIZE | SWP_NOZORDER);
     label_step.SetWindowPos(NULL, rect.right - 120, 60, 40, 30, SWP_NOSIZE | SWP_NOZORDER);
     buttonDrawSequence.SetWindowPos(NULL, rect.right - 150, 100, 120, 30, SWP_NOSIZE | SWP_NOZORDER);
     runAllButton.SetWindowPos(NULL, rect.right - 150, 140, 120, 30, SWP_NOSIZE | SWP_NOZORDER);
}

void CMyWindow::OnMyButtonClick()
{ // button 'Run once' pressed. It prints one Josephus permutation number at a time
    if (inputNumbers.empty()) {
        return;
    }
    runOncePressed = true;
    if (indexOfEliminationOrder < inputNumbers.size() - 1) {
        indexOfEliminationOrder++; // increment current index in Josephus permutation
        runAllPressed = false;

        Invalidate(true);
    }
}

void CMyWindow::OnButtonDrawSeqClick() { // button 'Draw seq.' pressed
    inputNumbers.clear();
    
    CString strTextStep;
    step.GetWindowText(strTextStep); // fetch text from  'step' text edit

    if (strTextStep.IsEmpty()) {
        AfxMessageBox(_T("Please speciffy Step field"));
        return;
    }
    std::size_t pos = 0;
    try {
        step_size = -1;
        // convert text to integer
        step_size = std::stoi(static_cast<const wchar_t*>(strTextStep), &pos, 10);
    }
    catch (const std::exception& ex) {
        AfxMessageBox(_T("Step field must be positive integer"));
        return;
    }
    if (pos != strTextStep.GetLength()) {
        AfxMessageBox(_T("Step field must be positive integer"));
        return;
    }

    CString strText;
    numbers.GetWindowText(strText); // fetch text from 'numbers' text edit
    int num = -1;
    if (strText.IsEmpty()) {
        AfxMessageBox(_T("Please speciffy num field"));
        return;
    }
    pos = 0;
    try {
        // convert text to integer
        num = std::stoi(static_cast<const wchar_t*>(strText), &pos, 10);
    }
    catch (const std::exception& ex) {
        AfxMessageBox(_T("Num field must be positive integer"));
        return;
    }
    if (pos != strText.GetLength()) {
        AfxMessageBox(_T("Num field must be positive integer"));
        return;
    }
    for (int i = 0; i <= num; ++i) {
        inputNumbers.push_back(i); // push back numbers in range [1,num] to vector
    }
    runAllPressed = false;
    runOncePressed = false;
    indexOfEliminationOrder = 0;

    // call Josephus permutation
    elimination_order = getJosephusSequence(num, step_size);

    Invalidate(TRUE); // invoke onPaint handler
}

void CMyWindow::OnButtonRunAll() { // button 'Run all' pressed. It prints whole Josephus permutation
    if (!inputNumbers.empty()) {
        runAllPressed = true;
        runOncePressed = false;
    }
    Invalidate(TRUE);
}

std::vector<int> getJosephusSequence(int n, int m) {
    if (n <= 0) return {};

    // Embedded doubly linked list representation
    // left[i] stores the person to the left of person i
    // right[i] stores the person to the right of person i
    std::vector<int> left(n + 1);
    std::vector<int> right(n + 1);

    // Initialize the circular links
    for (int i = 1; i <= n; ++i) {
        left[i] = (i == 1) ? n : i - 1;
        right[i] = (i == n) ? 1 : i + 1;
    }

    std::vector<int> elimination_order;
    elimination_order.reserve(n);

    // Start pointing at the first person
    int current = 1;

    for (int step = 0; step < n; ++step) {
        // Move forward m - 1 times. Since m is a constant, this takes O(1) time.
        for (int i = 0; i < m - 1; ++i) {
            current = right[current];
        }

        // Record the eliminated person
        elimination_order.push_back(current);

        // Remove 'current' from the circular list by updating its neighbors
        int prev_person = left[current];
        int next_person = right[current];

        right[prev_person] = next_person;
        left[next_person] = prev_person;

        // Move to the next active person for the subsequent iteration
        current = next_person;
    }

    return elimination_order;
}

// 3. Write the drawing code inside OnPaint
void CMyWindow::OnPaint()
{
    CPaintDC dc(this);

    if (inputNumbers.size() > 0) {
        int x = 20;
        int y = 20;
        CRect rect;
        GetClientRect(&rect);

        CFont myFont;
        myFont.CreatePointFont(150, _T("Times New Roman"));
        dc.SelectObject(&myFont);

        int screenWidth = rect.right - rect.left;
        for (int i = 1; i < inputNumbers.size(); ++i) {
            std::string num_str(std::to_string(inputNumbers[i]));
            CString cstr(num_str.c_str());
            dc.TextOut(x, y, cstr);
            if (runOncePressed) {
                std::vector<int> eliminationPartial(elimination_order.begin(), elimination_order.begin() + indexOfEliminationOrder);

                if (std::find(eliminationPartial.begin(), eliminationPartial.end(), i) != eliminationPartial.end()) {
                    COLORREF penColor = (eliminationPartial[indexOfEliminationOrder - 1] == i)
                        ? RGB(0, 0, 255)   // Blue
                        : RGB(0, 255, 0);  // Green
                    CPen pen(PS_SOLID, 3, penColor);
                    dc.SelectObject(&pen);

                        dc.SelectStockObject(NULL_BRUSH);

                    // Draw the circle
                        if (i >= 10) {
                            dc.Ellipse(x - 15, y - 5, x + 35, y + 35);
                        }
                        else {
                            dc.Ellipse(x - 15, y - 5, x + 25, y + 35);
                        }
                }
            }
            x += 50;
            if (x > screenWidth - 300) {
                y += 40;
                x = 20;
            }
        }
        

        if (runAllPressed) { // prints whole Josephus permutation
            runOncePressed = false;
            indexOfEliminationOrder = 0;
            heightOfInputNumbers = y;
            x = 20;
            y = y + 40;
            dc.TextOut(x, y, L"Josephus permutation:");
            CSize textSize = dc.GetTextExtent(L"Josephus permutation:");
            
            x += textSize.cx + 40;

            for (int i = 0; i < elimination_order.size(); ++i) {
                std::string num_str(std::to_string(elimination_order[i]));
                CString cstr(num_str.c_str());
                dc.TextOut(x, y, cstr);
                x += 50;
                if (x > screenWidth - 300) {
                    y += 40;
                    x = 20;
                }
            }
        }
        else if (runOncePressed) { // prints one Josephus number at a time
            runAllPressed = false;
            x = 20;
            y = y + 40;
            dc.TextOut(x, y, L"Josephus permutation:");
            CSize textSize = dc.GetTextExtent(L"Josephus permutation:");

            x += textSize.cx + 40;

            std::vector<int> eliminationPartial(elimination_order.begin(), elimination_order.begin() + indexOfEliminationOrder);

            for (int i = 0; i < eliminationPartial.size(); ++i) {
                std::string num_str(std::to_string(eliminationPartial[i]));
                CString cstr(num_str.c_str());
                dc.TextOut(x, y, cstr);
                x += 50;
                if (x > screenWidth - 300) {
                    y += 40;
                    x = 20;
                }
            }
        }

        myFont.DeleteObject();
    }
}

// 4. Standard App class initializing our custom window
class CEmptyMfcApp : public CWinApp
{
public:
    virtual BOOL InitInstance() override
    {
        CWinApp::InitInstance();

        CMyWindow* pFrame = new CMyWindow();
        m_pMainWnd = pFrame;
        pFrame->ShowWindow(SW_SHOW);
        pFrame->UpdateWindow();

        return TRUE;
    }
};

CEmptyMfcApp theApp;

// tomographic_projectionDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "tomographic_projection.h"
#include "tomographic_projectionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ctime"
#define _USE_MATH_DEFINES
#include "math.h"
#include <algorithm>

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CtomographicprojectionDlg



CtomographicprojectionDlg::CtomographicprojectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOMOGRAPHIC_PROJECTION_DIALOG, pParent),
	_maxAngle(360.0),
	_stepDistance(5.0),
	_stepAngle(5.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtomographicprojectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, _paintTomograph);
	DDX_Text(pDX, IDC_EDIT1, _stepDistance);
	DDX_Text(pDX, IDC_EDIT2, _stepAngle);
}

BEGIN_MESSAGE_MAP(CtomographicprojectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtomographicprojectionDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtomographicprojectionDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// Обработчики сообщений CtomographicprojectionDlg

BOOL CtomographicprojectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию


	UpdateData(FALSE);

	srand(static_cast<unsigned int>(time(NULL)));

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CtomographicprojectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CtomographicprojectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CtomographicprojectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/** Обработчик загрузки ихображения.*/
void CtomographicprojectionDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	uploadImage();

	UpdateData(FALSE);
}

/** Обработчик получения проекции.*/
void CtomographicprojectionDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);



	UpdateData(FALSE);
}

/** Загрузить изображение.*/
int CtomographicprojectionDlg::uploadImage()
{
	CFileDialog fd(true, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
		OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), NULL, 0, TRUE);

	if (fd.DoModal() != IDOK)
	{
		return IDCANCEL;
	};

	CString pathBMP = fd.GetPathName();
	Bitmap bmp(pathBMP);
	int width = bmp.GetWidth();
	int height = bmp.GetHeight();

	std::vector<Pixel> vecPixelsRow;
	_vecImage.clear();
	for (int i{ 0 }; i < height; ++i)
	{
		for (int j{ 0 }; j < width; ++j)
		{
			Pixel pixel;
			Color color;
			bmp.GetPixel(j, height - i - 1, &color);
			pixel.red = color.GetRed();
			pixel.green = color.GetGreen();
			pixel.blue = color.GetBlue();
			vecPixelsRow.emplace_back(pixel);
		}
		_vecImage.emplace_back(vecPixelsRow);
		vecPixelsRow.clear();
	}
	_radius = sqrt(static_cast<int>(width / 2) + static_cast<int>(height / 2));

	_paintTomograph.setImage(_vecImage);
	_paintTomograph.exchange(0, width, 0, height);

	_vecProjectionMatrix.clear();
	_vecExtendedImage.clear();
	extendImage(height, width, _vecExtendedImage, _vecProjectionMatrix);

	return IDOK;
}

/** Дополнить изображение нулями со всех сторон и заполнить матрицу проекции.
* @param heightOld - высота исходного изображения.
* @param widthOld - ширина исходного изображения.
* @param vecExtendedImage - расширенное изображение.
* @param vecProjectionMatrix - матрица проекции.
*/
void CtomographicprojectionDlg::extendImage(int heightOld,
	int widthOld,
	std::vector<std::vector<Pixel>> & vecExtendedImage,
	std::vector<std::vector<Pixel>> & vecProjectionMatrix)
{
	int diameter = _radius * _radius;

	std::vector<Pixel> vecOldImageRows;
	std::vector<Pixel> vecProjectionMatrixRows;

	int deltaHeight = static_cast<int>((diameter - heightOld) / 2);
	int deltaWidth = static_cast<int>((diameter - widthOld) / 2);

	for (std::size_t idxHeight{ 0U }; idxHeight < diameter; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < diameter; ++idxWidth)
		{
			Pixel pixel;
			if (idxHeight < deltaHeight || idxHeight >= deltaHeight + heightOld ||
				idxWidth < deltaWidth || idxWidth >= deltaWidth + widthOld)
			{
				pixel.red = 0.0;
				pixel.green = 0.0;
				pixel.blue = 0.0;
				vecOldImageRows.emplace_back(pixel);
			}
			else
			{
				vecOldImageRows.emplace_back(_vecImage[idxWidth][idxHeight]);
			}

			if (!(0U == idxWidth))
			{
				pixel.red = 0.0;
				pixel.green = 0.0;
				pixel.blue = 0.0;
				vecProjectionMatrixRows.emplace_back(pixel);
			}
			else
			{
				pixel.red = 255.0;
				pixel.green = 255.0;
				pixel.blue = 255.0;
				vecProjectionMatrixRows.emplace_back(pixel);
			}
		}
		vecExtendedImage.emplace_back(vecOldImageRows);
		vecOldImageRows.clear();

		vecProjectionMatrix.emplace_back(vecProjectionMatrixRows);
		vecProjectionMatrixRows.clear();
	}
}

/** Матрица поворота.
* @param x - координата x.
* @param y - координата y.
* @param angle - угол поворота.
*/
void CtomographicprojectionDlg::transformMatrix(double& x, double& y, double angle)
{
	double tmp_x = cos(angle * M_PI / 180) * 0.0 - sin(angle * M_PI / 180);
	double tmp_y = sin(angle * M_PI / 180) * x + cos(angle * M_PI / 180);

	x = tmp_x;
	y = tmp_y;
}

/** Повернуть изображение.
* @param angle - угол, на которое нужно повернуть исходное изображение.
* @param vecImageIn - исходное изображение.
* @param vecImageOut - повернутое изображение.
*/
void CtomographicprojectionDlg::rotateImage(double angle,
	const std::vector<std::vector<Pixel>> & vecImageIn,
	std::vector<std::vector<Pixel>> & vecImageOut)
{
	const std::size_t size = vecImageIn.size();
	vecImageOut.clear();
	std::vector<Pixel> vecImageRow(size);
	vecImageOut.resize(size, vecImageRow);

	Matrix matrix;
	Point pointNew;
	const Point center(size / 2, size / 2);
	matrix.Translate(center.X, center.Y);
	matrix.Rotate(angle);

	Rect rect(0, 0, size, size);
	for (std::size_t y{ 0U }; y < size; ++y)
	{
		for (std::size_t x{ 0U }; x < size; ++x)
		{
			pointNew.X = x - center.X;
			pointNew.Y = y - center.Y;
			matrix.TransformPoints(&pointNew);
			if (!rect.Contains(pointNew))
			{
				continue;
			}

			vecImageOut[y][x] = vecImageIn[pointNew.Y][pointNew.X];
		}
	}
}

/** Вычислить томографическую проекцию.*/
void CtomographicprojectionDlg::computeProjection()
{
	for (std::size_t y{ 0U }; y < size; ++y)
	{
		for (std::size_t x{ 0U }; x < size; ++x)
		{
			pointNew.X = x - center.X;
			pointNew.Y = y - center.Y;
			matrix.TransformPoints(&pointNew);
			if (!rect.Contains(pointNew))
			{
				continue;
			}

			vecImageOut[y][x] = vecImageIn[pointNew.Y][pointNew.X];
		}
	}
}

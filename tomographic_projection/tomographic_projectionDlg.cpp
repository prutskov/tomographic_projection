
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
	_maxAngle(180.0),
	_stepDistance(2),
	_stepAngle(2.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtomographicprojectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, _paintImage);
	DDX_Control(pDX, IDC_STATIC4, _paintTomographicProjection);
	DDX_Control(pDX, IDC_STATIC7, _paintRestoredImage);
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

	computeProjection();

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

	std::vector<double> vecPixelsRow;
	_vecImage.clear();
	for (int i{ 0 }; i < height; ++i)
	{
		for (int j{ 0 }; j < width; ++j)
		{
			double pixel = 0.0;
			Color color;
			bmp.GetPixel(j, height - i - 1, &color);
			pixel = 0.299 * color.GetRed() + 0.587 * color.GetGreen() + 0.114 * color.GetBlue();
			vecPixelsRow.emplace_back(pixel);
		}
		_vecImage.emplace_back(vecPixelsRow);
		vecPixelsRow.clear();
	}
	_radius = static_cast<int>(sqrt(static_cast<int>(width / 2) * static_cast<int>(width / 2) +
		static_cast<int>(height / 2) * static_cast<int>(height / 2)));

	_vecExtendedImage.clear();
	extendImage(height, width, _vecExtendedImage);

	_paintImage.setImage(_vecExtendedImage);
	_paintImage.exchange(0, _vecExtendedImage[0].size(), 0, _vecExtendedImage.size());

	return IDOK;
}

/** Дополнить изображение нулями со всех сторон.
* @param heightOld - высота исходного изображения.
* @param widthOld - ширина исходного изображения.
* @param vecExtendedImage - расширенное изображение.
*/
void CtomographicprojectionDlg::extendImage(int heightOld, int widthOld, std::vector<std::vector<double>> & vecExtendedImage)
{
	int diameter = 2 * _radius;
	std::vector<double> vecOldImageRow(diameter);
	// 1 parameter: colCount == width, 2 parameter: rowCount == height
	vecExtendedImage.resize(diameter, vecOldImageRow);

	for (std::size_t idxHeight{ 0U }; idxHeight < heightOld; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < widthOld; ++idxWidth)
		{
			vecExtendedImage[idxHeight + _radius - heightOld / 2][idxWidth + _radius - widthOld / 2] = _vecImage[idxHeight][idxWidth];
		}
	}
	vecOldImageRow.clear();
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
	const std::vector<std::vector<double>> & vecImageIn,
	std::vector<std::vector<double>> & vecImageOut)
{
	int size = static_cast<int>(vecImageIn.size());

	vecImageOut.clear();
	std::vector<double> vecImageRow(size);
	// 1 parameter: colCount == width, 2 parameter: rowCount == height
	vecImageOut.resize(size, vecImageRow);

	Matrix matrix;
	Point newPoint;
	const Point center(static_cast<int>(size / 2), static_cast<int>(size / 2));
	matrix.Translate(static_cast<Gdiplus::REAL>(center.X), static_cast<Gdiplus::REAL>(center.Y));
	matrix.Rotate(static_cast<Gdiplus::REAL>(angle));

	Rect rect(0, 0, size, size);
	for (int y{ 0 }; y < size; ++y)
	{
		for (int x{ 0 }; x < size; ++x)
		{
			newPoint.X = x - center.X;
			newPoint.Y = y - center.Y;
			matrix.TransformPoints(&newPoint);
			if (!rect.Contains(newPoint))
			{
				continue;
			}

			vecImageOut[y][x] = vecImageIn[newPoint.Y][newPoint.X];
		}
	}
}

/** Вычислить томографическую проекцию.*/
void CtomographicprojectionDlg::computeProjection()
{
	std::size_t height = _vecExtendedImage.size();
	std::size_t width = _vecExtendedImage[0].size();

	_vecProjection.clear();
	std::vector<std::vector<double>> vecRotatedImage;
	std::vector<double> vecProjectionRow;
	for (double idxAngle{ 0.0 }; idxAngle < _maxAngle; idxAngle += _stepAngle)
	{
		rotateImage(idxAngle, _vecExtendedImage, vecRotatedImage);

		// проход вдоль S
		int distancePoint = 0;
		while (distancePoint < width)
		{
			double pixel = 0.0;

			for (int idxHeight{ 0 }; idxHeight < height; ++idxHeight)
			{
				pixel += vecRotatedImage[idxHeight][distancePoint];
			}

			vecProjectionRow.emplace_back(pixel);
			distancePoint += _stepDistance;
		}

		_vecProjection.emplace_back(vecProjectionRow);
		vecProjectionRow.clear();
	}

	height = _vecProjection.size();
	width = _vecProjection[0].size();
	std::vector<double> vecMaxs;
	for (std::size_t idxAngle{ 0U }; idxAngle < height; ++idxAngle)
	{
		auto it = std::max_element(_vecProjection[idxAngle].begin(), _vecProjection[idxAngle].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxEl = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (double & el : _vecProjection[idxHeight])
		{ 
			el = el / *maxEl * 255.0; 
		}
	}

	_paintTomographicProjection.setImage(_vecProjection);
	_paintTomographicProjection.exchange(0, static_cast<double>(_vecProjection[0].size()), 0, static_cast<double>(_vecProjection.size()));
}

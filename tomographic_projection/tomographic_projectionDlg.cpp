
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
	_stepDistance(5),
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
	DDX_Control(pDX, IDC_STATIC9, _paintFFT);
	DDX_Control(pDX, IDC_STATIC11, _paintFFTTranslated);
	DDX_Control(pDX, IDC_STATIC13, _paintFFT2D);
	DDX_Text(pDX, IDC_EDIT1, _stepDistance);
	DDX_Text(pDX, IDC_EDIT2, _stepAngle);
}

BEGIN_MESSAGE_MAP(CtomographicprojectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtomographicprojectionDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtomographicprojectionDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CtomographicprojectionDlg::OnBnClickedButton3)
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

	std::size_t oldHeight = _vecImage.size();
	std::size_t oldWidth = _vecImage[0].size();

	std::size_t newHeight = pow_2(static_cast<unsigned int>(oldHeight));
	std::size_t newWidth = pow_2(static_cast<unsigned int>(oldWidth));

	std::vector<std::vector<std::complex<double>>> vecImageInterpolated;
	vecImageInterpolated.resize(newHeight, std::vector<std::complex<double>>(newWidth, 0.0));

	for (std::size_t idxHeight{ 0U }; idxHeight < oldHeight; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < oldWidth; ++idxWidth)
		{
			vecImageInterpolated[idxHeight][idxWidth].real(_vecImage[idxHeight][idxWidth]);
		}
	}

	fourier2D(vecImageInterpolated, -1);

	_vecFFT2D.clear();
	std::vector<double> vec_spectrum_abs1D;
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			vec_spectrum_abs1D.push_back(log(1 + vecImageInterpolated[idxWidth][idxHeight].real() * vecImageInterpolated[idxWidth][idxHeight].real() +
				vecImageInterpolated[idxWidth][idxHeight].imag() * vecImageInterpolated[idxWidth][idxHeight].imag()));
		}
		_vecFFT2D.push_back(vec_spectrum_abs1D);
		vec_spectrum_abs1D.clear();
	}

	/** Трансформация спектра(перемещние спектра в центр).*/
	double tmp = 0.0;
	for (std::size_t i{ 0U }; i < newHeight; ++i)
	{
		for (std::size_t j{ 0U }; j < newWidth / 2; ++j)
		{
			tmp = _vecFFT2D[i][j];
			_vecFFT2D[i][j] = _vecFFT2D[i][j + newWidth / 2];
			_vecFFT2D[i][j + newWidth / 2] = tmp;
		}
	}
	for (std::size_t i{ 0U }; i < newHeight / 2; ++i)
	{
		for (std::size_t j{ 0U }; j < newWidth; ++j)
		{
			tmp = _vecFFT2D[i][j];
			_vecFFT2D[i][j] = _vecFFT2D[newHeight / 2 + i][j];
			_vecFFT2D[newHeight / 2 + i][j] = tmp;
		}
	}

	imageNormalization(_vecFFT2D);

	_paintImage.setImage(_vecExtendedImage);
	_paintImage.exchange(0.0, static_cast<double>(_vecExtendedImage[0].size()), 0.0, static_cast<double>(_vecExtendedImage.size()));

	_paintFFT2D.setFFT2D(_vecFFT2D);
	_paintFFT2D.exchange(0.0, static_cast<double>(newWidth), 0.0, static_cast<double>(newHeight));

	UpdateData(FALSE);
}

/** Обработчик получения проекции.*/
void CtomographicprojectionDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	// computeProjection();

	int pointsNumber = _vecImage[0].size() / _stepDistance;
	int anglesNumber = _maxAngle / _stepAngle;
	std::vector<Projection> vecRadonTransform = getRadonTransform(_vecImage, anglesNumber, pointsNumber);
	std::vector<std::vector<double>> vecProjections;
	vecProjections.resize(vecRadonTransform.size());
	for (std::size_t i{ 0U }; i < vecProjections.size(); ++i)
	{
		vecProjections[i] = vecRadonTransform[i].vecSpecificProjection;
	}

	imageNormalization(vecProjections);

	_paintTomographicProjection.setProjection(vecProjections);
	_paintRestoredImage.exchange(0.0, static_cast<double>(vecProjections[0].size()), 
		0.0, static_cast<double>(vecProjections.size()));

	UpdateData(FALSE);
}

/** Обработчик получения восстановленного изображения.*/
void CtomographicprojectionDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);

	fourierFromProjection();

	std::size_t height = _vecProjectionFFT.size();
	std::size_t width = _vecProjectionFFT[0].size();

	fourier2D(_vecProjectionFFT, 1);

	_vecImageRestored.clear();
	std::vector<double> vecImageRestoredRow;

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < width; ++idxWidth)
		{
			double val = _vecProjectionFFT[idxWidth][idxHeight].real();
			val = (val < 0.0) ? 0.0 : val;
			vecImageRestoredRow.emplace_back(val);
		}
		_vecImageRestored.emplace_back(vecImageRestoredRow);
		vecImageRestoredRow.clear();
	}

	// permute FFT
	double tmp{ 0.0 };
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < width / 2; ++idxWidth)
		{
			tmp = _vecImageRestored[idxHeight][idxWidth];
			_vecImageRestored[idxHeight][idxWidth] = _vecImageRestored[idxHeight][idxWidth + width / 2];
			_vecImageRestored[idxHeight][idxWidth + width / 2] = tmp;
		}
	}
	for (std::size_t idxHeight{ 0U }; idxHeight < height / 2; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < width; ++idxWidth)
		{
			tmp = _vecImageRestored[idxHeight][idxWidth];
			_vecImageRestored[idxHeight][idxWidth] = _vecImageRestored[height / 2 + idxHeight][idxWidth];
			_vecImageRestored[height / 2 + idxHeight][idxWidth] = tmp;
		}
	}

	imageNormalization(_vecImageRestored);

	_paintRestoredImage.setImageRestored(_vecImageRestored);
	_paintRestoredImage.exchange(0.0, static_cast<double>(width), 0.0, static_cast<double>(height));

	UpdateData(FALSE);
}

/** Привести изображение к диапазону от 0 до 255.
* @param image - редактируемое изображение.
*/
void CtomographicprojectionDlg::imageNormalization(std::vector<std::vector<double>> & image)
{
	std::size_t height = image.size();
	std::size_t width = image[0].size();

	std::vector<double> vecMaxs;
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(image[idxHeight].begin(), image[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxEl = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (double & el : image[idxHeight])
		{
			el = el / *maxEl * 255.0;
		}
	}
	vecMaxs.clear();
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
			bmp.GetPixel(j, i, &color);
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
	// 1 parameter: rowCount == height, 2 parameter: colCount == width
	vecExtendedImage.resize(diameter, std::vector<double>(diameter, 0.0));

	for (std::size_t idxHeight{ 0U }; idxHeight < heightOld; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < widthOld; ++idxWidth)
		{
			vecExtendedImage[idxHeight + _radius - heightOld / 2][idxWidth + _radius - widthOld / 2] = _vecImage[idxHeight][idxWidth];
		}
	}
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
	int height = static_cast<int>(vecImageIn.size());
	int width = static_cast<int>(vecImageIn[0].size());

	vecImageOut.clear();
	// 1 parameter: rowCount == height, 2 parameter: colCount == width
	vecImageOut.resize(height, std::vector<double>(width, 0.0));

	Matrix matrix{};
	Point newPoint{};
	const Point center(static_cast<int>(width / 2), static_cast<int>(height / 2));
	matrix.Translate(static_cast<Gdiplus::REAL>(center.X), static_cast<Gdiplus::REAL>(center.Y));
	matrix.Rotate(static_cast<Gdiplus::REAL>(angle));

	Rect rect(0, 0, width, height);
	for (int idxHeight{ 0 }; idxHeight < height; ++idxHeight)
	{
		for (int idxWidth{ 0 }; idxWidth < width; ++idxWidth)
		{
			newPoint.X = idxWidth - center.X;
			newPoint.Y = idxHeight - center.Y;
			matrix.TransformPoints(&newPoint);

			if (!rect.Contains(newPoint))
			{
				continue;
			}

			vecImageOut[idxHeight][idxWidth] = vecImageIn[newPoint.Y][newPoint.X];
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
	for (double angle{ 0.0 }; angle <= _maxAngle; angle += _stepAngle)
	{
		rotateImage(angle, _vecExtendedImage, vecRotatedImage);

		// проход вдоль S, лучи просвечивают объект вертикально
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
	vecRotatedImage.clear();

	height = _vecProjection.size();
	width = _vecProjection[0].size();
	std::vector<double> vecMaxs;
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(_vecProjection[idxHeight].begin(), _vecProjection[idxHeight].end());
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
	vecMaxs.clear();

	_paintTomographicProjection.setProjection(_vecProjection);
	_paintTomographicProjection.exchange(0.0, static_cast<double>(width), 0.0, static_cast<double>(height));
}

/** Найти ближайшее сверху число к x, равное степени двойки.
* @param x - значение, которое необходимо дополнить до степени двойки.
* @return - значение, равное степени двойки.
*/
unsigned int CtomographicprojectionDlg::pow_2(unsigned int x)
{
	x--;
	for (unsigned int p = 1; p < 32; p <<= 1)
		x |= (x >> p);
	return ++x;
}

/** Произвести одномерное ПФ.
* @param data - данные, над котромыми производится ПФ.
* @param n - размер данных.
* @param is - прямое/обратное ПФ (-1 или 1).
*/
void CtomographicprojectionDlg::fourier1D(std::vector<std::complex<double>> *data, int n, int is)
{
	int i, j, istep;
	int m, mmax;
	double r, r1, theta, w_r, w_i, temp_r, temp_i;
	double pi = M_PI;

	r = pi * is;
	j = 0;
	for (i = 0; i < n; ++i)
	{
		if (i < j)
		{
			temp_r = (*data)[j].real();
			temp_i = (*data)[j].imag();
			(*data)[j] = std::complex<double>((*data)[i].real(), (*data)[i].imag());
			(*data)[i] = std::complex<double>(temp_r, temp_i);
		}
		m = n >> 1;
		while (j >= m)
		{
			j -= m; m = (m + 1) / 2;
		}
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (double)mmax;
		for (m = 0; m < mmax; ++m)
		{
			theta = r1 * m;
			w_r = (double)cos((double)theta);
			w_i = (double)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * (*data)[j].real() - w_i * (*data)[j].imag();
				temp_i = w_r * (*data)[j].imag() + w_i * (*data)[j].real();
				(*data)[j] = std::complex<double>((*data)[i].real() - temp_r, (*data)[i].imag() - temp_i);
				(*data)[i] += std::complex<double>(temp_r, temp_i);
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; ++i)
		{
			(*data)[i] = std::complex<double>((*data)[i].real() / (double)n, (*data)[i].imag() / double(n));
		}
}

/** Произвести двухмерное ПФ.
* @param data - данные, над которыми производится ПФ.
* @param is - прямое/обратное ПФ (-1 или 1).
*/
void CtomographicprojectionDlg::fourier2D(std::vector<std::vector<std::complex<double>>> &data, int is)
{
	int height = static_cast<int>(data.size());
	int width = static_cast<int>(data[0].size());

	// ПФ над каждой строкой
#pragma omp parallel for
	for (int i = 0; i < height; ++i)
	{
		fourier1D(&data[i], width, is);
	}

	std::vector<std::vector<std::complex<double>>> vec_columns;
	for (int i{ 0 }; i < width; ++i)
	{
		std::vector<std::complex<double>> vec_tmp;
		for (int j{ 0 }; j < height; ++j)
		{
			vec_tmp.push_back(data[j][i]);
		}
		vec_columns.push_back(vec_tmp);
	}

#pragma omp parallel for
	for (int i = 0; i < width; ++i)
	{
		fourier1D(&vec_columns[i], height, is);
	}

	data.clear();
	data = vec_columns;
}

/** Вычислить ПФ от томографической проекции.*/
void CtomographicprojectionDlg::fourierFromProjection()
{
	std::size_t height = _vecProjection.size();   // axis of angles
	std::size_t width = _vecProjection[0].size();   // axis of S

	std::size_t newWidth = static_cast<std::size_t>(pow_2(static_cast<unsigned int>(width)));   // for FFT

	std::vector<std::vector<std::complex<double>>> vecProjectionCmplx;
	vecProjectionCmplx.resize(height, std::vector<std::complex<double>>(newWidth, { 0.0, 0.0 }));

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			if (idxWidth < width)
			{
				vecProjectionCmplx[idxHeight][idxWidth] = std::complex<double>(_vecProjection[idxHeight][idxWidth], 0.0);
			}
			else
			{
				vecProjectionCmplx[idxHeight][idxWidth] = std::complex<double>(0.0, 0.0);
			}
		}
		// FFT forward for angle is equal to idxHeight degree
		fourier1D(&vecProjectionCmplx[idxHeight], static_cast<int>(newWidth), -1);
	}

	// permute FFT
	std::complex<double> tmp{ 0.0, 0.0 };
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth / 2; ++idxWidth)
		{
			tmp = vecProjectionCmplx[idxHeight][idxWidth];
			vecProjectionCmplx[idxHeight][idxWidth] = vecProjectionCmplx[idxHeight][idxWidth + newWidth / 2];
			vecProjectionCmplx[idxHeight][idxWidth + newWidth / 2] = tmp;
		}
	}
	for (std::size_t idxHeight{ 0U }; idxHeight < height / 2; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			tmp = vecProjectionCmplx[idxHeight][idxWidth];
			vecProjectionCmplx[idxHeight][idxWidth] = vecProjectionCmplx[height / 2 + idxHeight][idxWidth];
			vecProjectionCmplx[height / 2 + idxHeight][idxWidth] = tmp;
		}
	}

	// abs
	_vecFFT.clear();
	_vecFFT.resize(height, std::vector<double>(newWidth, 0.0));
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			double val = vecProjectionCmplx[idxHeight][idxWidth].real() * vecProjectionCmplx[idxHeight][idxWidth].real() +
				vecProjectionCmplx[idxHeight][idxWidth].imag() * vecProjectionCmplx[idxHeight][idxWidth].imag();
			_vecFFT[idxHeight][idxWidth] = log(1 + val);
		}
	}

	std::vector<double> vecMaxs;
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(_vecFFT[idxHeight].begin(), _vecFFT[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxElFFT = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (double & el : _vecFFT[idxHeight])
		{
			el = el / *maxElFFT * 255.0;
		}
	}

	_paintFFT.setFFT(_vecFFT);
	_paintFFT.exchange(0.0, static_cast<double>(newWidth), 0.0, static_cast<double>(height));
	_vecFFT.clear();

	_vecProjectionFFT.clear();
	// std::size_t newHeight = static_cast<std::size_t>(pow_2(static_cast<unsigned int>(height)));
	std::size_t newHeight = newWidth;
	_vecProjectionFFT.resize(newHeight, std::vector<std::complex<double>>(newWidth, { 0.0, 0.0 }));

	interpolateNearNeigh(_vecProjectionFFT,
		vecProjectionCmplx,
		static_cast<int>(newWidth),
		static_cast<int>(height),
		static_cast<int>(newWidth),
		static_cast<int>(newHeight));

	vecProjectionCmplx.clear();

	// gaussian
	int h = (int)newHeight;
	int w = (int)newWidth;
	double radius = static_cast<double>(sqrt(w / 2 * w / 2 + h / 2 * h / 2));
	for (int idxHeight{ 0 }; idxHeight < h; ++idxHeight)
	{
		for (int idxWidth{ 0 }; idxWidth < w; ++idxWidth)
		{
			_vecProjectionFFT[idxHeight][idxWidth].real(_vecProjectionFFT[idxHeight][idxWidth].real() *
				(exp(-((idxWidth - w / 2) * (idxWidth - w / 2) + (idxHeight - h / 2) * (idxHeight - h / 2)) /
				((radius / 1.5) * (radius / 1.5) + (radius / 1.5) * (radius / 1.5)))));
			_vecProjectionFFT[idxHeight][idxWidth].imag(_vecProjectionFFT[idxHeight][idxWidth].imag() *
				(exp(-((idxWidth - w / 2) * (idxWidth - w / 2) + (idxHeight - h / 2) * (idxHeight - h / 2)) /
				((radius / 1.5) * (radius / 1.5) + (radius / 1.5) * (radius / 1.5)))));
		}
	}

	for (int idxHeight{ 0 }; idxHeight < h; ++idxHeight)
	{
		for (int idxWidth{ 0 }; idxWidth < w; ++idxWidth)
		{
			_vecProjectionFFT[idxHeight][idxWidth].real(_vecProjectionFFT[idxHeight][idxWidth].real() * (1 -
				(exp(-((idxWidth - w / 2) * (idxWidth - w / 2) + (idxHeight - h / 2) * (idxHeight - h / 2)) /
				((radius / 3.0) * (radius / 3.0) + (radius / 3.0) * (radius / 3.0))))));
			_vecProjectionFFT[idxHeight][idxWidth].imag(_vecProjectionFFT[idxHeight][idxWidth].imag() * (1 -
				(exp(-((idxWidth - w / 2) * (idxWidth - w / 2) + (idxHeight - h / 2) * (idxHeight - h / 2)) /
				((radius / 3.0) * (radius / 3.0) + (radius / 3.0) * (radius / 3.0))))));
		}
	}

	auto newVector = _vecProjectionFFT;

	// transpose
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			_vecProjectionFFT[idxHeight][idxWidth] = newVector[idxWidth][idxHeight];
		}
	}

	// butterworth
	/*int radius = static_cast<int>(sqrt(newWidth * newWidth + newHeight * newHeight));
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight / 2; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			double D = sqrt(idxHeight * idxHeight + idxWidth * idxWidth);
			_vecProjectionFFT[idxHeight][idxWidth].real(_vecProjectionFFT[idxHeight][idxWidth].real() / (1 + (D / radius) * (D / radius)));
			_vecProjectionFFT[idxHeight][idxWidth].imag(_vecProjectionFFT[idxHeight][idxWidth].imag() / (1 + (D / radius) * (D / radius)));
		}
	}*/

	// FFT after interpolation
	_vecFFTTranslated.clear();
	_vecFFTTranslated.resize(newHeight, std::vector<double>(newWidth, 0.0));
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			double val = _vecProjectionFFT[idxHeight][idxWidth].real() * _vecProjectionFFT[idxHeight][idxWidth].real() +
				_vecProjectionFFT[idxHeight][idxWidth].imag() * _vecProjectionFFT[idxHeight][idxWidth].imag();
			_vecFFTTranslated[idxHeight][idxWidth] = log(1 + val);
		}
	}

	vecMaxs.clear();
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		auto it = std::max_element(_vecFFTTranslated[idxHeight].begin(), _vecFFTTranslated[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxElFFTTrasnlated = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		for (double & el : _vecFFTTranslated[idxHeight])
		{
			el = el / *maxElFFTTrasnlated * 255.0;
		}
	}

	_paintFFTTranslated.setFFTTranslated(_vecFFTTranslated);
	_paintFFTTranslated.exchange(0.0, static_cast<double>(newWidth), 0.0, static_cast<double>(newHeight));

	// permute FFT
	std::complex<double> tmp2{ 0.0, 0.0 };
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth / 2; ++idxWidth)
		{
			tmp2 = _vecProjectionFFT[idxHeight][idxWidth];
			_vecProjectionFFT[idxHeight][idxWidth] = _vecProjectionFFT[idxHeight][idxWidth + newWidth / 2];
			_vecProjectionFFT[idxHeight][idxWidth + newWidth / 2] = tmp2;
		}
	}
	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight / 2; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			tmp2 = _vecProjectionFFT[idxHeight][idxWidth];
			_vecProjectionFFT[idxHeight][idxWidth] = _vecProjectionFFT[newHeight / 2 + idxHeight][idxWidth];
			_vecProjectionFFT[newHeight / 2 + idxHeight][idxWidth] = tmp2;
		}
	}
}

void CtomographicprojectionDlg::interpolationMKL(std::vector<std::vector<std::complex<double>>> & vecRes,
	std::vector<std::vector<std::complex<double>>> & vecIn)
{
	std::size_t height = vecIn.size();
	std::size_t width = vecIn[0].size();

	std::size_t newHeight = vecRes.size();
	std::size_t newWidth = vecRes[0].size();

	//radonImage is the result of r2c FFT
	//RT(pp,:) contains frequences 0...q
	std::complex<double> *RT = (std::complex<double>*)(vecIn.data());
	int q = (static_cast<int>(width) - 1) / 2; // w = 2q + 1
	int ldq = static_cast<int>(width) / 2;
	int p = static_cast<int>(height);

	for (int i = 0; i < newHeight; ++i)
	{
		for (int j = 0; j < newWidth; ++j)
		{
			double yy = 2.0 * i / newHeight - 1; // yy = [-1...1]
			double xx = 2.0 * j / newWidth - 1; // xx = [-1...1]
			double r = sqrt(xx * xx + yy * yy);
			double phi = atan2(yy, xx);
			std::complex<double> fhat_ij{ 0.0, 0.0 };
			if (r <= 1)
			{
				if (phi < 0.0)
				{
					r = -r;
					phi += M_PI;
				}

				int qq = static_cast<int>(floor(q + r * q + 0.5) - q); // qq = [-q...q)
				if (qq >= q) qq = q - 1;

				int pp = static_cast<int>(floor(phi / M_PI * p + 0.5)); // pp = [0...p)
				if (pp >= p) pp = p - 1;


				if (qq >= 0)
					fhat_ij = RT[pp * ldq + qq];
				else
					fhat_ij = std::conj(RT[pp * ldq - qq]);

				if (isOdd(qq)) fhat_ij = -fhat_ij;
				if (isOdd(i)) fhat_ij = -fhat_ij;
				if (isOdd(j)) fhat_ij = -fhat_ij;
			}
			vecRes[i][j] = fhat_ij;
		}
	}
}

/** Интерполировать, методом ближайшего соседа.*/
void CtomographicprojectionDlg::interpolateNearNeigh(std::vector<std::vector<std::complex<double>>> & vecOut,
	const std::vector<std::vector<std::complex<double>>> & vecIn,
	int w1,
	int h1,
	int w2,
	int h2)
{
	for (int i{ 0 }; i < h2; ++i)
	{
		for (int j{ 0 }; j < w2; ++j)
		{
			int newY = i - h2 / 2; // per angle
			int newX = j - w2 / 2; // per S

			double theta = 0.0;
			// index of new point on theta
			if (!(0.0 == newY) || !(0.0 == newX))
			{
				theta = static_cast<double>(atan2(static_cast<double>(newY), static_cast<double>(newX)));
			}
			else
			{
				theta = 0.0;
			}
			if (theta <= 0.0)
			{
				theta += M_PI;
			}
			// index of new point on s
			int s = static_cast<int>(std::hypot(newX, newY));
			// index of old point0 on theta
			int idxTheta0 = static_cast<int>(std::floor(theta / (_stepAngle / 180.0F * M_PI)));
			// index of old point1 on theta
			int idxTheta1 = static_cast<int>(std::ceil(theta / (_stepAngle / 180.0F * M_PI)));

			/*if (idxTheta0 == idxTheta1)
			{
				continue;
			}*/

			double midTheta = theta / (_stepAngle / 180.0 * M_PI);

			// if theta0 and theta1 are less than array size on angle axis
			// and are more than 0-index, and if s is less than half of array size on S axis
			if ((idxTheta0 < h1) && (idxTheta1 < h1) &&
				(idxTheta0 >= 0) && (idxTheta1 >= 0) && (s < w1 / 2))
			{
				if (std::fabs(midTheta - idxTheta0) < std::fabs(midTheta - idxTheta1))
				{
					if ((theta - M_PI) <= 0.0)
					{
						vecOut[i][j] = vecIn[idxTheta0][-s + w1 / 2 - 1];
					}
					else
					{
						vecOut[i][j] = vecIn[idxTheta0][s + w1 / 2 - 1];
					}
				}
				else
				{
					if ((theta - M_PI) <= 0.0)
					{
						vecOut[i][j] = vecIn[idxTheta1][-s + w1 / 2 - 1];
					}
					else
					{
						vecOut[i][j] = vecIn[idxTheta1][s + w1 / 2 - 1];
					}
				}
			}
		}
	}
}

/** Преобразовать координаты s, t в x, y.
* @param s - координата по s.
* @param t - координата по t.
* @param x - координата по Ох.
* @param y - координата по Оу.
* @param theta - угол поворота.
*/
void CtomographicprojectionDlg::coordTransform(double s, double t, int&x, int&y, double theta)
{
	x = (int)(s * cos(theta) - t * sin(theta));
	y = (int)(s * sin(theta) + t * cos(theta));
}

Projection CtomographicprojectionDlg::getSpecificProjection(const std::vector<std::vector<double>> & img, 
	double angle, int n)
{
	int width = img.size();
	int height = img[0].size();
	int d = static_cast<int>((sqrt(width / 2 * width / 2 + height / 2 * height / 2)));
	int step = (int)(2 * d / n);
	if (step < 1)
		step = 1;

	Projection projection;
	int x, y;
	for (int s = -d; s < d; s += step)
	{
		std::vector<std::vector<int>> vecCoeffs(2 * d, std::vector<int>(2 * d, 0));
		double sum = 0.0;
		for (int t = -d; t < d; ++t)
		{
			coordTransform(s, t, x, y, angle);
			int i = width / 2 + x;
			int j = height / 2 + y;
			if ((i >= 0) && (i < width) && (j >= 0) && (j < height))
			{
				sum += img[i][j];
			}
			if ((d + x >= 0) && (d + x < 2 * d) && (d + y >= 0) && (d + y < 2 * d))
			{
				vecCoeffs[d + x][d + y] = 1;
			}
		}

		projection.s.push_back(s);
		projection.vecSpecificProjection.push_back(sum);
		projection.vecCoeffs.push_back(vecCoeffs);
	}

	projection.theta = angle;
	return projection;
}

/** Вычислить преобразование Радона.
* @param vecImage - изображение для вычисления преобразования.
* @param numberAngles - число углов.
* @param numberPoints - число точек.
*/
std::vector<Projection> CtomographicprojectionDlg::getRadonTransform(const std::vector<std::vector<double>> & vecImage, 
	int anglesNumber, int pointsNumber)
{
	std::vector<Projection> vecProjections;
	vecProjections.resize(anglesNumber);
	double step_angle = M_PI / anglesNumber;

	for (int i = 0; i < anglesNumber; ++i)
	{
		vecProjections[i] = getSpecificProjection(vecImage, i * step_angle, pointsNumber);
	}

	return vecProjections;
}

/** Метод Качмаржа.
* @param a - вытянутая в строку матрица весовых коэффициентов.
* @param b - вектор правой части.
* @param x - буфер под запись решения.
* @param nn - количество неизвестных.
* @param ny - количество уравнений.
*/
void CtomographicprojectionDlg::kaczmarzMethod(double* a, double* b, double* x, int nn, int ny)
{
	// nn - количество неизвестных;  ny - количество уравнений
	double eps = 1.e-6;
	//float s;
	int i, j, k;
	double s1, s2, fa1, t;
	double *x1;

	x1 = new double[nn];

	x[0] = 0.5;
	for (i = 1; i < nn; ++i)
		x[i] = 0.0;

	s1 = s2 = 1.0;
	while (s1 > eps * s2)
	{
		for (i = 0; i < nn; ++i)
			x1[i] = x[i];

		for (i = 0; i < ny; ++i)
		{
			s1 = 0.0;
			s2 = 0.0;
			for (j = 0; j < nn; ++j)
			{
				fa1 = a[i * nn + j];
				s1 += fa1 * x[j];
				s2 += fa1 * fa1;
			}
			t = (b[i] - s1) / s2;
			for (k = 0; k < nn; ++k)
				x[k] += a[i * nn + k] * t;
		}

		s1 = 0.0;
		s2 = 0.0;
		for (i = 0; i < nn; ++i)
		{
			s1 += (x[i] - x1[i]) * (x[i] - x1[i]);
			s2 += x[i] * x[i];
		}
		s1 = (double)sqrt(s1);
		s2 = (double)sqrt(s2);
	}
	delete[] x1;
}
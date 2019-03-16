
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
	_maxAngle(180.0F),
	_stepDistance(2),
	_stepAngle(2.0F)
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

	UpdateData(FALSE);
}

/** Обработчик получения проекции.*/
void CtomographicprojectionDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	computeProjection();

	UpdateData(FALSE);
}

void CtomographicprojectionDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);

	fourierFromProjection();

	std::size_t height = _vecProjectionFFT.size();
	std::size_t width = _vecProjectionFFT[0].size();

	std::size_t newHeight = pow_2(height);
	std::vector<std::vector<cmplx>> vecResImg;
	vecResImg.resize(newHeight, std::vector<cmplx>(width, { 0.0F, 0.0F }));

	interpolateNearNeigh(vecResImg, _vecProjectionFFT, width, height, width, newHeight);

	fourier2D(vecResImg, 1);

	_vecImageRestored.clear();
	std::vector<float> vecImageRestoredRow;

	for (std::size_t idxHeight{ 0U }; idxHeight < newHeight; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < width; ++idxWidth)
		{
			float val = vecResImg[idxWidth][idxHeight].real;
			val = val < 0.0F ? 0.0F : (val > 255.0F ? 255 : val);
			vecImageRestoredRow.emplace_back(val);
		}
		_vecImageRestored.emplace_back(vecImageRestoredRow);
		vecImageRestoredRow.clear();
	}

	height = _vecImageRestored.size();
	width = _vecImageRestored[0].size();

	std::vector<float> vecMaxs;
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(_vecImageRestored[idxHeight].begin(), _vecImageRestored[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxEl = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (float & el : _vecImageRestored[idxHeight])
		{
			el = el / *maxEl * 255.0F;
		}
	}
	vecMaxs.clear();

	_paintRestoredImage.setImageRestored(_vecImageRestored);
	_paintRestoredImage.exchange(0.0F, static_cast<float>(width), 0.0F, static_cast<float>(height));

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

	std::vector<float> vecPixelsRow;
	_vecImage.clear();
	for (int i{ 0 }; i < height; ++i)
	{
		for (int j{ 0 }; j < width; ++j)
		{
			float pixel = 0.0F;
			Color color;
			bmp.GetPixel(j, i, &color);
			pixel = 0.299F * color.GetRed() + 0.587F * color.GetGreen() + 0.114F * color.GetBlue();
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
	_paintImage.exchange(0.0F, static_cast<float>(_vecExtendedImage[0].size()), 0.0F, static_cast<float>(_vecExtendedImage.size()));

	return IDOK;
}

/** Дополнить изображение нулями со всех сторон.
* @param heightOld - высота исходного изображения.
* @param widthOld - ширина исходного изображения.
* @param vecExtendedImage - расширенное изображение.
*/
void CtomographicprojectionDlg::extendImage(int heightOld, int widthOld, std::vector<std::vector<float>> & vecExtendedImage)
{
	int diameter = 2 * _radius;
	// 1 parameter: rowCount == height, 2 parameter: colCount == width
	vecExtendedImage.resize(diameter, std::vector<float>(diameter, 0.0F));

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
void CtomographicprojectionDlg::rotateImage(float angle,
	const std::vector<std::vector<float>> & vecImageIn,
	std::vector<std::vector<float>> & vecImageOut)
{
	int height = static_cast<int>(vecImageIn.size());
	int width = static_cast<int>(vecImageIn[0].size());

	vecImageOut.clear();
	// 1 parameter: rowCount == height, 2 parameter: colCount == width
	vecImageOut.resize(height, std::vector<float>(width, 0.0F));

	Matrix matrix{};
	Point newPoint{};
	const Point center(static_cast<int>(height / 2), static_cast<int>(width / 2));
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

/** Повернуть ПФ.
* @param angle - угол, на которое нужно повернуть исходное изображение.
* @param vecImageIn - исходное изображение.
* @param vecImageOut - повернутое изображение.
*/
void CtomographicprojectionDlg::rotateFFT(float angle,
	const std::vector<std::vector<cmplx>> & vecImageIn,
	std::vector<std::vector<cmplx>> & vecImageOut)
{
	int height = static_cast<int>(vecImageIn.size());
	int width = static_cast<int>(vecImageIn[0].size());

	vecImageOut.clear();
	// 1 parameter: rowCount == height, 2 parameter: colCount == width
	vecImageOut.resize(height, std::vector<cmplx>(width, { 0.0F, 0.0F }));

	Matrix matrix{};
	Point newPoint{};
	const Point center(static_cast<int>(height / 2), static_cast<int>(width / 2));
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
	std::vector<std::vector<float>> vecRotatedImage;
	std::vector<float> vecProjectionRow;
	for (float angle{ 0.0F }; angle < _maxAngle; angle += _stepAngle)
	{
		rotateImage(angle, _vecExtendedImage, vecRotatedImage);

		// проход вдоль S, лучи просвечивают объект вертикально
		int distancePoint = 0;
		while (distancePoint < width)
		{
			float pixel = 0.0F;

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
	std::vector<float> vecMaxs;
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(_vecProjection[idxHeight].begin(), _vecProjection[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxEl = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (float & el : _vecProjection[idxHeight])
		{
			el = el / *maxEl * 255.0F;
		}
	}
	vecMaxs.clear();

	_paintTomographicProjection.setProjection(_vecProjection);
	_paintTomographicProjection.exchange(0.0F, static_cast<float>(width), 0.0F, static_cast<float>(height));
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
void CtomographicprojectionDlg::fourier1D(std::vector<cmplx> *data, int n, int is)
{
	int i, j, istep;
	int m, mmax;
	float r, r1, theta, w_r, w_i, temp_r, temp_i;
	float pi = static_cast<float>(M_PI);

	r = pi * is;
	j = 0;
	for (i = 0; i < n; ++i)
	{
		if (i < j)
		{
			temp_r = (*data)[j].real;
			temp_i = (*data)[j].image;
			(*data)[j].real = (*data)[i].real;
			(*data)[j].image = (*data)[i].image;
			(*data)[i].real = temp_r;
			(*data)[i].image = temp_i;
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
		r1 = r / (float)mmax;
		for (m = 0; m < mmax; ++m)
		{
			theta = r1 * m;
			w_r = (float)cos((float)theta);
			w_i = (float)sin((float)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * (*data)[j].real - w_i * (*data)[j].image;
				temp_i = w_r * (*data)[j].image + w_i * (*data)[j].real;
				(*data)[j].real = (*data)[i].real - temp_r;
				(*data)[j].image = (*data)[i].image - temp_i;
				(*data)[i].real += temp_r;
				(*data)[i].image += temp_i;
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; ++i)
		{
			(*data)[i].real /= (float)n;
			(*data)[i].image /= (float)n;
		}
}

/** Произвести двухмерное ПФ.
* @param data - данные, над которыми производится ПФ.
* @param is - прямое/обратное ПФ (-1 или 1).
*/
void CtomographicprojectionDlg::fourier2D(std::vector<std::vector<cmplx>> &data, int is)
{
	int height = static_cast<int>(data.size());
	int width = static_cast<int>(data[0].size());

	// ПФ над каждой строкой
#pragma omp parallel for
	for (int i{ 0 }; i < height; ++i)
	{
		fourier1D(&data[i], width, is);
	}

	std::vector<std::vector<cmplx>> vec_columns;
	for (int i{ 0 }; i < width; ++i)
	{
		std::vector<cmplx> vec_tmp;
		for (int j{ 0 }; j < height; ++j)
		{
			vec_tmp.push_back(data[j][i]);
		}
		vec_columns.push_back(vec_tmp);
	}

#pragma omp parallel for
	for (int i{ 0 }; i < width; ++i)
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

	std::vector<std::vector<cmplx>> vecProjectionCmplx;
	vecProjectionCmplx.resize(height, std::vector<cmplx>(newWidth, { 0.0F, 0.0F }));

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			if (idxWidth < width)
			{
				vecProjectionCmplx[idxHeight][idxWidth].real = _vecProjection[idxHeight][idxWidth];
				vecProjectionCmplx[idxHeight][idxWidth].image = 0.0F;
			}
			else
			{
				vecProjectionCmplx[idxHeight][idxWidth].real = 0.0F;
				vecProjectionCmplx[idxHeight][idxWidth].image = 0.0F;
			}
		}
		// FFT forward for angle is equal to idxHeight degree
		fourier1D(&vecProjectionCmplx[idxHeight], static_cast<int>(newWidth), -1);
	}

	_vecFFT.resize(height, std::vector<float>(newWidth, 0.0F));
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			float val = vecProjectionCmplx[idxHeight][idxWidth].real * vecProjectionCmplx[idxHeight][idxWidth].real +
				vecProjectionCmplx[idxHeight][idxWidth].image * vecProjectionCmplx[idxHeight][idxWidth].image;
			_vecFFT[idxHeight][idxWidth] = log(1 + val);
		}
	}

	std::vector<std::vector<float>> vecFFTBuffer;
	vecFFTBuffer.resize(height, std::vector<float>(newWidth, 0.0F));
	vecFFTBuffer = _vecFFT;

	std::vector<float> vecMaxs;
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(vecFFTBuffer[idxHeight].begin(), vecFFTBuffer[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxElFFT = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (float & el : vecFFTBuffer[idxHeight])
		{
			el = el / *maxElFFT * 255.0F;
		}
	}

	_paintFFT.setFFT(vecFFTBuffer);
	_paintFFT.exchange(0.0F, static_cast<float>(newWidth), 0.0F, static_cast<float>(height));

	// FFT before rotating per angle
	std::vector<std::vector<cmplx>> vecProjectionCmplxBufferIn;
	vecProjectionCmplxBufferIn.resize(height, std::vector<cmplx>(newWidth, { 0.0F, 0.0F }));

	// FFT after rotating per angle
	std::vector<std::vector<cmplx>> vecProjectionCmplxBufferOut;
	vecProjectionCmplxBufferOut.resize(height, std::vector<cmplx>(newWidth, { 0.0F, 0.0F }));

	_vecProjectionFFT.resize(height, std::vector<cmplx>(newWidth, { 0.0, 0.0 }));

	std::size_t idxAngle{ 0U };
	for (float angle{ 0.0 }; angle < _maxAngle; angle += _stepAngle)
	{
		// record middle line to rotate her
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			vecProjectionCmplxBufferIn[height / 2][idxWidth] = vecProjectionCmplx[idxAngle][idxWidth];
		}

		rotateFFT(angle, vecProjectionCmplxBufferIn, vecProjectionCmplxBufferOut);

		// FFT arrays is built to star
		for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
		{
			for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
			{
				if (!(newWidth / 2 == idxWidth) && !(height / 2 == idxHeight))
				{
					_vecProjectionFFT[idxHeight][idxWidth].real += vecProjectionCmplxBufferOut[idxHeight][idxWidth].real;
					_vecProjectionFFT[idxHeight][idxWidth].image += vecProjectionCmplxBufferOut[idxHeight][idxWidth].image;
				}
			}
		}

		++idxAngle;
	}

	_vecFFTTranslated.resize(height, std::vector<float>(newWidth, 0.0F));
	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < newWidth; ++idxWidth)
		{
			float val = _vecProjectionFFT[idxHeight][idxWidth].real * _vecProjectionFFT[idxHeight][idxWidth].real +
				_vecProjectionFFT[idxHeight][idxWidth].image * _vecProjectionFFT[idxHeight][idxWidth].image;
			_vecFFTTranslated[idxHeight][idxWidth] = log(1 + val);
		}
	}

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		auto it = std::max_element(_vecFFTTranslated[idxHeight].begin(), _vecFFTTranslated[idxHeight].end());
		vecMaxs.emplace_back(*it);
	}

	auto maxElFFTTrasnlated = std::max_element(vecMaxs.begin(), vecMaxs.end());

	for (std::size_t idxHeight{ 0U }; idxHeight < height; ++idxHeight)
	{
		for (float & el : _vecFFTTranslated[idxHeight])
		{
			el = el / *maxElFFTTrasnlated * 255.0F;
		}
	}

	_paintFFTTranslated.setFFTTranslated(_vecFFTTranslated);
	_paintFFTTranslated.exchange(0.0F, static_cast<float>(newWidth), 0.0F, static_cast<BYTE>(height));
}

//void step2_interpolation(std::vector<std::vector<cmplx>> & vecRes, std::vector<std::vector<cmplx>> & vecIn)
//{
//	std::size_t oldHeight = vecIn.size();
//	std::size_t oldWidth = vecIn[0].size();
//
//	std::size_t newHeight = vecRes.size();
//	std::size_t newWidth = vecRes[0].size();
//
//	int q = oldWidth / 2;
//	int ldq = oldWidth / 2;
//	int p = oldHeight;
//
//	for (int i = 0; i < newHeight; ++i)
//	{
//		for (int j = 0; j < newWidth; ++j)
//		{
//			float yy = 2.0*i / newHeight - 1; // yy = [-1...1]
//			float xx = 2.0*j / newWidth - 1; // xx = [-1...1]
//			float r = sqrt(xx * xx + yy * yy);
//			float phi = atan2(yy, xx);
//			cmplx fhat_ij{0.0F, 0.0F};
//			if (r <= 1)
//			{
//				if (phi < 0)
//				{
//					r = -r;
//					phi += M_PI;
//				}
//
//				int qq = floor(q + r * q + 0.5) - q; // qq = [-q...q)
//				if (qq >= q) 
//					qq = q - 1;
//
//				int pp = floor(phi / M_PI * p + 0.5); // pp = [0...p)
//				if (pp >= p) 
//					pp = p - 1;
//
//
//				if (qq >= 0)
//				{
//					fhat_ij = vecIn[pp * ldq + qq];
//				}
//				else
//					fhat_ij = conj(RT[pp*ldq - qq]);
//
//				if (isOdd(qq)) 
//					fhat_ij = -fhat_ij;
//				if (isOdd(i)) 
//					fhat_ij = -fhat_ij;
//				if (isOdd(j)) 
//					fhat_ij = -fhat_ij;
//			}
//			fhat->data[i*fhat->ldw + j] = fhat_ij;
//		}
//	}
//}


//void nearest_neighbor(IF &img, std::size_t zoom)
//{
//	std::size_t width = img.width(),
//		height = img.height();
//
//	IF tmp(width, height*zoom, 1, 3, 0);
//
//	for (std::size_t i = 0; i < width; i++)
//	{
//		for (std::size_t j = 0; j < height; j++)
//		{
//			for (std::size_t k = 0; k < zoom; k++)
//			{
//				tmp(i, j*zoom + k, 0) = img(i, j, 0);
//				tmp(i, j*zoom + k, 1) = img(i, j, 1);
//				tmp(i, j*zoom + k, 2) = img(i, j, 2);
//			}
//		}
//	}
//
//	img = tmp;
//	IF tmp2(width*zoom, height*zoom, 1, 3, 0);
//	for (ST i = 0; i < width; i++)
//	{
//		for (ST j = 0; j < height*zoom; j++)
//		{
//			for (ST k = 0; k < zoom; k++)
//			{
//				tmp2(i*zoom + k, j, 0) = img(i, j, 0);
//				tmp2(i*zoom + k, j, 1) = img(i, j, 1);
//				tmp2(i*zoom + k, j, 2) = img(i, j, 2);
//			}
//		}
//	}
//	tmp2.display();
//}

/** Интерполировать, методом ближайшего соседа.*/
void CtomographicprojectionDlg::interpolateNearNeigh(std::vector<std::vector<cmplx>> & vecOut,
	std::vector<std::vector<cmplx>> & vecIn, 
	int w1, 
	int h1,
	int w2, 
	int h2) 
{
	float x_ratio = w1 / (float)w2;
	float y_ratio = h1 / (float)h2;
	float px, py;
	for (int i = 0; i < h2; i++) 
	{
		for (int j = 0; j < w2; j++) 
		{
			px = int(floor(j * x_ratio));
			py = int(floor(i * y_ratio));
			vecOut[i][j].real = vecIn[py][px].real;
			vecOut[i][j].image = vecIn[py][px].image;
		}
	}
}
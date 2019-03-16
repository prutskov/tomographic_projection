
// tomographic_projectionDlg.h: файл заголовка
//

#pragma once

#include <vector>
#include "Paint.h"


using namespace Gdiplus;

// Диалоговое окно CtomographicprojectionDlg
class CtomographicprojectionDlg : public CDialogEx
{
	// Создание
public:
	CtomographicprojectionDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOMOGRAPHIC_PROJECTION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	/** Загрузить изображение.*/
	int uploadImage();

	/** Дополнить изображение нулями со всех сторон.
	* @param heightOld - высота исходного изображения.
	* @param widthOld - ширина исходного изображения.
	* @param vecExtendedImage - расширенное изображение.
	*/
	void extendImage(int heightOld, int widthOld, std::vector<std::vector<float>> & vecExtendedImage);

	/** Повернуть изображение.
	* @param angle - угол, на которое нужно повернуть исходное изображение.
	* @param vecImageIn - исходное изображение.
	* @param vecImageOut - повернутое изображение.
	*/
	void rotateImage(float angle, const std::vector<std::vector<float>> & vecImageIn, std::vector<std::vector<float>> & vecImageOut);

	/** Повернуть ПФ.
	* @param angle - угол, на которое нужно повернуть исходное изображение.
	* @param vecImageIn - исходное изображение.
	* @param vecImageOut - повернутое изображение.
	*/
	void rotateFFT(float angle, 
		const std::vector<std::vector<std::complex<float>>> & vecImageIn, std::vector<std::vector<std::complex<float>>> & vecImageOut);

	/** Вычислить томографическую проекцию.*/
	void computeProjection();

	/** Найти ближайшее сверху число к x, равное степени двойки.
	* @param x - значение, которое необходимо дополнить до степени двойки.
	* @return - значение, равное степени двойки.
	*/
	unsigned int pow_2(unsigned int x);

	/** Произвести одномерное ПФ.
	* @param data - данные, над котромыми производится ПФ.
	* @param n - размер данных.
	* @param is - прямое/обратное ПФ (-1 или 1).
	*/
	void fourier1D(std::vector<std::complex<float>> *data, int n, int is);

	/** Произвести двухмерное ПФ.
	* @param data - данные, над которыми производится ПФ.
	* @param is - прямое/обратное ПФ (-1 или 1).
	*/
	void CtomographicprojectionDlg::fourier2D(std::vector<std::vector<std::complex<float>>> &data, int is);

	/** Вычислить ПФ от томографической проекции.*/
	void fourierFromProjection();

	/** Интерполировать, методом ближайшего соседа.*/
	void interpolateNearNeigh(std::vector<std::vector<std::complex<float>>> & vecOut,
		std::vector<std::vector<std::complex<float>>> & vecIn,
		int w1,
		int h1,
		int w2,
		int h2);

	/** Интерполировать, методом из MKL.*/
	void interpolationMKL(std::vector<std::vector<std::complex<float>>> & vecRes, std::vector<std::vector<std::complex<float>>> & vecIn);

	template<typename T> 
	inline int isOdd(const T& n) 
	{ 
		return n & 1;
	};

	/** Повернуть точку.
	* @param angle - угол, на которое нужно повернуть исходное изображение.
	* @param x - координата по х.
	* @param y - координата по у.
	*/
	void CtomographicprojectionDlg::rotatePoint(float angle,
		int width,
		int height,
		std::complex<float> & xCoord,
		std::complex<float> & yCoord);


private:
	/** Отрисовщик исходного изображения.*/
	Paint _paintImage;
	/** Отрисовщик томографической проекции.*/
	Paint _paintTomographicProjection;
	/** Отрисовщик восстановленного изображения.*/
	Paint _paintRestoredImage;
	/** Отрисовщик FFT.*/
	Paint _paintFFT;
	/** Отрисовщик FFT translated.*/
	Paint _paintFFTTranslated;

	/** Изображение(сигнал).*/
	std::vector<std::vector<float>> _vecImage;
	/** Расширенное изображение(сигнал).*/
	std::vector<std::vector<float>> _vecExtendedImage;
	/** Томографическая проекция.*/
	std::vector<std::vector<float>> _vecProjection;
	/** FFT от томографической проекции.*/
	std::vector<std::vector<std::complex<float>>> _vecProjectionFFT;
	/** Восстановленное изображение(сигнал).*/
	std::vector<std::vector<float>> _vecImageRestored;
	/** FFT.*/
	std::vector<std::vector<float>> _vecFFT;
	/** FFT translated.*/
	std::vector<std::vector<float>> _vecFFTTranslated;

	/** Радиус окружности, описанной вокруг изображения.*/
	int _radius;
	/** Максимальный угол.*/
	const float _maxAngle;
	/** Угловой шаг.*/
	float _stepAngle;
	/** Линейный шаг.*/
	int _stepDistance;


public:
	/** Обработчик загрузки изображения.*/
	afx_msg void OnBnClickedButton1();
	/** Обработчик получения проекции.*/
	afx_msg void OnBnClickedButton2();
	/** Обработчик получения восстановленного изображения.*/
	afx_msg void OnBnClickedButton3();
};

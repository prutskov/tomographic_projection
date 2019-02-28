
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

	/** Дополнить изображение нулями со всех сторон и заполнить матрицу проекции.
	* @param heightOld - высота исходного изображения.
	* @param widthOld - ширина исходного изображения.
	* @param vecExtendedImage - расширенное изображение.
	* @param vecProjectionMatrix - матрица проекции.
	*/
	void extendImage(int heightOld, 
					 int widthOld, 
					 std::vector<std::vector<Pixel>> & vecExtendedImage, 
					 std::vector<std::vector<Pixel>> & vecProjectionMatrix);

	/** Повернуть изображение.
	* @param angle - угол, на которое нужно повернуть исходное изображение.
	* @param vecImageIn - исходное изображение.
	* @param vecImageOut - повернутое изображение.
	*/
	void rotateImage(double angle, const std::vector<std::vector<Pixel>> & vecImageIn, std::vector<std::vector<Pixel>> & vecImageOut);

	/** Матрица поворота.
	* @param x - координата x.
	* @param y - координата y.
	* @param angle - угол поворота.
	*/
	void transformMatrix(double& x, double& y, double angle);

	/** Вычислить томографическую проекцию.*/
	void computeProjection();
	

private:
	/** Отрисовщик.*/
	Paint _paintTomograph;
	/** Изображение(сигнал).*/
	std::vector<std::vector<Pixel>> _vecImage;
	/** Расширенное изображение(сигнал).*/
	std::vector<std::vector<Pixel>> _vecExtendedImage;
	/** Матрица проекции.*/
	std::vector<std::vector<Pixel>> _vecProjectionMatrix;
	/** Радиус окружности, описанной вокруг изображения.*/
	int _radius;
	/** Максимальный угол.*/
	const double _maxAngle;
	/** Угловой шаг.*/
	double _stepAngle;
	/** Линейный шаг.*/
	double _stepDistance;
	

public:
	/** Обработчик загрузки ихображения.*/
	afx_msg void OnBnClickedButton1();
	/** Обработчик получения проекции.*/
	afx_msg void OnBnClickedButton2();
};

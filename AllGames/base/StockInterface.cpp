#include "DeepStock.h"
#include "StockInterface.h"
CStock		g_stock;
void		RenderStock(IRender* r, int bufferWidth, int bufferHeight)
{
	VALUE fLow=100.0;
	VALUE fHigh=0;
	int stockWidth=3;
	int nWidth = 0;
	int begin = g_stock.m_days.size()-100;
	for(int i=begin;i<g_stock.m_days.size();i++){
		StockDay& day = g_stock.m_days[i];
		if(day.Low<fLow)
			fLow=day.Low;
		if(day.High>fHigh)
			fHigh=day.High;
		nWidth += (stockWidth+1);
		if(nWidth>=bufferWidth)
			break;
	}
	nWidth = 0;
	//fHigh = (VALUE)bufferHeight/(VALUE)stockWidth / 100.0f * fLow;
	VALUE fValueRange = (VALUE)bufferHeight / (fHigh - fLow);
	for(int i=begin;i<g_stock.m_days.size();i++){
		StockDay& day = g_stock.m_days[i];
		int nOpen = bufferHeight - (day.Open-fLow)*fValueRange;
		int nClose = bufferHeight - (day.Close-fLow)*fValueRange;
		//
		int color = 0xffff0000;
		if(day.Close < day.Open)
			color = 0xff00ffaf;
		r->DrawRect(nWidth, nClose>nOpen?nOpen:nClose, stockWidth, nClose>nOpen?(nClose-nOpen):nOpen-nClose, color);
		//
		nWidth += (stockWidth+1);
		if(nWidth>=bufferWidth)
			break;
	}
}
void		LoadStock(const char* szFile)
{
	g_stock.LoadBinary(szFile);

}


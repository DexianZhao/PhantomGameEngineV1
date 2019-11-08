
class IRender
{
public:
	virtual void	DrawRect(int x, int y, int w, int h, int color) = 0;
	//void	DrawLine(int x1, int y1, int x2, int y2, int color);
};

void		RenderStock(IRender* r, int bufferWidth, int bufferHeight);
void		LoadStock(const char* szFile);


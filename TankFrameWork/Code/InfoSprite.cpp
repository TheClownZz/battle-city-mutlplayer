#include "InfoSprite.h"

InfoSprite::InfoSprite(const char* path)
{
	ReadXML(path);
}

InfoSprite::InfoSprite(float frameW, float frameH, int frameCount, int framePerRow)
{
	this->Width = framePerRow*frameW;
	this->Height = (frameCount/ framePerRow)*frameH;
	this->frameW = frameW;
	this->frameH = frameH;
	this->FrameCount = frameCount;
	this->FramePerRow = framePerRow;

	//Vòng lặp lấy thông tin từng frame gán vào mảng Struct
	for (int i = 0; i < frameCount; i++)
	{

		this->InfoFrame[i].x = (i % framePerRow)*(frameW);
		this->InfoFrame[i].y = (i / framePerRow)*(frameH);
		this->InfoFrame[i].w = frameW;
		this->InfoFrame[i].h = frameH;
		this->InfoFrame[i].pX = 0.5f;
		this->InfoFrame[i].pY = 0.5f;
		this->InfoFrame[i].sX = 0.0f;
		this->InfoFrame[i].sY = 0.0f;
	}
}

InfoSprite::~InfoSprite()
{
}


void  InfoSprite::ReadXML(const char* path)
{
	//Lấy tài liệu xml
	TiXmlDocument doc(path);
	if (!doc.LoadFile()) //Đọc XML
	{
		MessageBox(NULL, "Can't load Sprite xml file", "Error", MB_OK);
		return;
	}
	//Lấy thông tin Xml
	TiXmlElement* sprite = doc.RootElement();//Gán node gốc
	TiXmlElement* indexml = nullptr; //tạo node trống 
	//Lấy thông tin kích thước sprite Texture
	sprite->QueryFloatAttribute("width", &this->Width);
	sprite->QueryFloatAttribute("height", &this->Height);
	//Lấy kích thích frame mặc định
	sprite->QueryFloatAttribute("framew", &this->frameW);
	sprite->QueryFloatAttribute("frameh", &this->frameH);
	//Lấy thông tin từng index
	sprite->QueryIntAttribute("frame", &this->FrameCount);
	int n;
	//Vòng lặp lấy thông tin từng frame gán vào mảng Struct
	for (indexml = sprite->FirstChildElement(); //bắt đầu từ frame đầu tròn file xml
		indexml != NULL; //Kết thúc khi không thấy node nào nữa
		indexml = indexml->NextSiblingElement() //Tiến tới node kế khi xong 1 vòng lặp
		)
	{
		indexml->QueryIntAttribute("n", &n);//n là thứ tự frame
		indexml->QueryFloatAttribute("x", &this->InfoFrame[n].x);
		indexml->QueryFloatAttribute("y", &this->InfoFrame[n].y);
		indexml->QueryFloatAttribute("w", &this->InfoFrame[n].w);
		indexml->QueryFloatAttribute("h", &this->InfoFrame[n].h);
		indexml->QueryFloatAttribute("pX", &this->InfoFrame[n].pX);
		indexml->QueryFloatAttribute("pY", &this->InfoFrame[n].pY);
		indexml->QueryFloatAttribute("sX", &this->InfoFrame[n].sX);
		indexml->QueryFloatAttribute("sY", &this->InfoFrame[n].sY);
	}
}

float InfoSprite::GetWidth()
{
	return this->Width;
}
float InfoSprite::GetHeight()
{
	return this->Height;
}

float InfoSprite::GetFrameWidth()
{
	return this->frameW;
}
float InfoSprite::GetFrameHeight()
{
	return this->frameH;
}

//Tạo kích thước frame khi có thông tin
RECT InfoSprite::GetRect(int Index)//index là STT của frame
{
	RECT rect;
	rect.left = InfoFrame[Index].x;//Cạnh trái bằng x
	rect.top = InfoFrame[Index].y;//Cạnh Trên bằng y
	rect.right = rect.left + InfoFrame[Index].w; //cạnh phải bằng trái cộng thêm chiều rộng
	rect.bottom = rect.top + InfoFrame[Index].h;//cạnh đấy bằng trên cộng thêm chiều cao (vì Vẽ lên View và tọa độ View hướng xuống nên top bé hơn bot)
	return rect;
}

//Lấy Vector Orgin để chỉnh vị trí phù hợp mặc định thường 0.5 0.5 ở giữa
D3DXVECTOR2 InfoSprite::GetOrgin(int Index)
{
	return D3DXVECTOR2(InfoFrame[Index].pX, InfoFrame[Index].pY);
}
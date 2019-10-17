#include <SoftwareSerial.h>

class SmartSensor
{
	public:
        SmartSensor();
        ~SmartSensor();

        void begin();
        void end();

        bool isDetected();  // 是否偵測到物件
        int getFuncID();    // 偵測物的功能ID
        int getTypeID();    // 該功能有Type才有值 (顏色 or 形狀ID)
        int getPosX();      // 偵測到物件的中心位置x座標
        int getPosY();      // 偵測到物件的中心位置y座標
        int getH();         // 物件高度
        int getW();         // 物件寬度

	private:
	    void serialFlush();
	    bool openCam();
	    bool isCamOpened;
	    bool bSendStreamOn;
	    bool hasDelayed;
	    int  nOpenCamFailCount;
	    int  nJsonErrCount;

	    int m_id;
	    int m_type;
	    int m_x;
	    int m_y;
	    int m_h;
	    int m_w;
};

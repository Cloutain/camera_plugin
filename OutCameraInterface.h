#ifndef _OUT_CAMERA_H_
#define _OUT_CAMERA_H_
	/**
	 * Camera ��ʼ��
	 * @param mCameraID ���ID  ���������0��ǰ�������1
	 * @return falseʧ�ܣ�true�ɹ�
	 */
	bool InitCamera(int mCameraID);
	
	
	/**
	 * ����Ԥ�� is_preview=1:��ʼԤ�� is_preview =0��ֹͣԤ��
	 */
	void SetPreview(int  is_preview);
	
	
	/**
	�������ʹ��ģʽ
	0: Ƕ�� unity ��rec ����ʶ�����
	1: Ƕ�� aradk.net, �Լ�������ͷ����
    */
	void SetMode(int mode)//δʵ��
	
	
	/** GetRenderEventFunc, 
	an example function we export which is used to get a rendering event callback function.
	*/
	void GetRenderEventFunc()
	
	
	/**�����������unity����ԭ����̬����ʱ�Զ�����*/
	void  UnityPluginLoad (UnityGfxDeviceEventType eventType)
	
	
	/**�����������unityж��ԭ����̬����ʱ�Զ�����*/
	void UnityPluginUnload()
	
	/**��������*/
	void  UpdateTexture()

	/**��������id
	textureHandle:����id
	w��unity����������
	h:unity��������߶�
	*/
	void SetTextureFromUnity(void* textureHandle, int w, int h)
	
	/**�ر����*/
	void CloseCamera()
	
	
	/** UpdateTexture�����õ�ʱ��
	unity��Ҫ����GetImg������������ȡdll�е�buffer����
	*/
	void GetImg(char** pImg, char** pGray)
	
	
	/** SetTimeFromUnity, ���������Ԥ��ʱ��
	an example function we export which is called by one of the scripts.
	*/
	void SetTimeFromUnity(float time)
}



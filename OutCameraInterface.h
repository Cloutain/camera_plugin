#ifndef _OUT_CAMERA_H_
#define _OUT_CAMERA_H_
	/**
	 * Camera 初始化
	 * @param mCameraID 相机ID  后置相机：0，前置相机：1
	 * @return false失败，true成功
	 */
	bool InitCamera(int mCameraID);
	
	
	/**
	 * 设置预览 is_preview=1:开始预览 is_preview =0：停止预览
	 */
	void SetPreview(int  is_preview);
	
	
	/**
	设置相机使用模式
	0: 嵌入 unity ，rec 调用识别核心
	1: 嵌入 aradk.net, 自己控制摄头控制
    */
	void SetMode(int mode)//未实现
	
	
	/** GetRenderEventFunc, 
	an example function we export which is used to get a rendering event callback function.
	*/
	void GetRenderEventFunc()
	
	
	/**这个函数是在unity加载原生动态库插件时自动调用*/
	void  UnityPluginLoad (UnityGfxDeviceEventType eventType)
	
	
	/**这个函数是在unity卸载原生动态库插件时自动调用*/
	void UnityPluginUnload()
	
	/**更新纹理*/
	void  UpdateTexture()

	/**设置纹理id
	textureHandle:纹理id
	w：unity传的纹理宽度
	h:unity传的纹理高度
	*/
	void SetTextureFromUnity(void* textureHandle, int w, int h)
	
	/**关闭相机*/
	void CloseCamera()
	
	
	/** UpdateTexture被调用的时候，
	unity需要调用GetImg（）函数，获取dll中的buffer数据
	*/
	void GetImg(char** pImg, char** pGray)
	
	
	/** SetTimeFromUnity, 设置相机的预览时间
	an example function we export which is called by one of the scripts.
	*/
	void SetTimeFromUnity(float time)
}



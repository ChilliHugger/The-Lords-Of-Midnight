package com.chillihugger.extensions;

import org.cocos2dx.lib.Cocos2dxActivity;

public class Utils
{
    public static String getBuildNo() {
        try {
            String version;
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
                return "" + Cocos2dxActivity.getContext().getPackageManager().getPackageInfo(Cocos2dxActivity.getContext().getPackageName(), 0).getLongVersionCode();
            }else{
                //noinspection deprecation
                return "" + Cocos2dxActivity.getContext().getPackageManager().getPackageInfo(Cocos2dxActivity.getContext().getPackageName(), 0).versionCode;
            }
        } catch(Exception e) {
            return "";
        }
    }
}

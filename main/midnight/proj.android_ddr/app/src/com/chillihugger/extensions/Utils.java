
package com.chillihugger.extensions;

import org.cocos2dx.lib.Cocos2dxActivity;

public class Utils
{
    public static String getBuildNo() {
        try {
            String version = "" + Cocos2dxActivity.getContext().getPackageManager().getPackageInfo(Cocos2dxActivity.getContext().getPackageName(), 0).versionCode;
            return version;
        } catch(Exception e) {
            return "";
        }
    }
}

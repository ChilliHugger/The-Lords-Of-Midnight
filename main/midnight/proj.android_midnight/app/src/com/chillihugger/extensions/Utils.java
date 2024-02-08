package com.chillihugger.extensions;

import android.content.Context;
import android.content.pm.PackageInfo;

import com.chilli.midnight.BuildConfig;

import org.axmol.lib.AxmolActivity;

public class Utils
{
    public static String getBuildNo() {
        try {
            return String.valueOf(BuildConfig.VERSION_CODE);
//            Context context = AxmolActivity.getContext();
//            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
//
//            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
//                return String.valueOf(packageInfo.getLongVersionCode());
//            }else{
//                //noinspection deprecation
//                return String.valueOf(packageInfo.versionCode);
//            }
        } catch(Exception e) {
            return "";
        }
    }
}

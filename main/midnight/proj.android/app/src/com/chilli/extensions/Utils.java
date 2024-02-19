package com.chilli.extensions;

import com.chilli.midnight.BuildConfig;

public class Utils
{
    public static String getBuildNo() {
        try {
            return String.valueOf(BuildConfig.VERSION_CODE);
        } catch(Exception e) {
            return "";
        }
    }
}

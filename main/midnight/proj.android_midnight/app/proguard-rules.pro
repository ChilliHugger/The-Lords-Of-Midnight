# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in E:\developSoftware\Android\SDK/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Proguard Cocos2d-x for release
-keep public class org.axmol.** { *; }
-dontwarn org.axmol.**
-keep public class com.chukong.** { *; }
-dontwarn com.chukong.**
-keep public class com.huawei.android.** { *; }
-dontwarn com.huawei.android.**

# Proguard Apache HTTP for release
-keep class org.apache.http.** { *; }
-dontwarn org.apache.http.**

# Proguard rules for TME
-keep public class com.chilli.extensions.** { *; }
-dontwarn com.chilli.extensions.**

# Proguard rules for AXMOL
-keep public class dev.axmol.lib.**
-keepclassmembers public class dev.axmol.lib.** { *; }
-dontwarn dev.axmol.**

# Proguard Android Webivew for release. uncomment if you are using a webview in cocos2d-x
#-keep public class android.net.http.SslError
#-keep public class android.webkit.WebViewClient

#-dontwarn android.webkit.WebView
#-dontwarn android.net.http.SslError
#-dontwarn android.webkit.WebViewClient
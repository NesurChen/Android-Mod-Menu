bool iconValid, settingsValid, initValid;

//Big letter cause crash
void setText(JNIEnv *env, jobject obj, const char* text){
    //https://stackoverflow.com/a/33627640/3763113
    //A little JNI calls here. You really really need a great knowledge if you want to play with JNI stuff
    //Html.fromHtml("");
    jclass html = (*env).FindClass(OBFUSCATE("android/text/Html"));
    jmethodID fromHtml = (*env).GetStaticMethodID(html, OBFUSCATE("fromHtml"), OBFUSCATE("(Ljava/lang/String;)Landroid/text/Spanned;"));

    //setText("");
    jclass textView = (*env).FindClass(OBFUSCATE("android/widget/TextView"));
    jmethodID setText = (*env).GetMethodID(textView, OBFUSCATE("setText"), OBFUSCATE("(Ljava/lang/CharSequence;)V"));

    //Java string
    jstring jstr = (*env).NewStringUTF(text);
    (*env).CallVoidMethod(obj, setText,  (*env).CallStaticObjectMethod(html, fromHtml, jstr));
}

jstring Icon(JNIEnv *env, jobject thiz) {
    iconValid = true;

    //Use https://www.base64encode.org/ to encode your image to base64
    return env->NewStringUTF(
            OBFUSCATE("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"));
}

jstring IconWebViewData(JNIEnv *env, jobject thiz) {
    iconValid = true;
    //WebView support GIF animation. Upload your image or GIF on imgur.com or other sites

    // From internet (Requires android.permission.INTERNET)
    // return env->NewStringUTF(OBFUSCATE("https://i.imgur.com/SujJ85j.gif"));

    // Base64 html:
    // return env->NewStringUTF("data:image/png;base64, <encoded base64 here>");

    // To disable it, return NULL. It will use normal image above:
    // return NULL

    //return env->NewStringUTF(OBFUSCATE_KEY("https://i.imgur.com/SujJ85j.gif", 'u'));
    return NULL;
}

jobjectArray SettingsList(JNIEnv *env, jobject activityObject) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Settings"),
            OBFUSCATE("-1_Toggle_Save feature preferences"), //-1 is checked on Preferences.java
            OBFUSCATE("-3_Toggle_Auto size vertically"),
            OBFUSCATE("Category_Menu"),
            OBFUSCATE("-6_Button_<font color='red'>Close settings</font>"),
    };

    int Total_Feature = (sizeof features /
                         sizeof features[0]); //Now you dont have to manually update the number everytime;
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));
    int i;
    for (i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    settingsValid = true;

    return (ret);
}

using namespace std;
#include <list>
#include <string.h>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"
bool modWuXing = false, freeAdver = false, stopSave = false, addItem =false, addProduce,
         keepYoung = false, notRemoveItem = false, forceUseItem = false, forceProduce = false;
int itemId = -1;
int* (*FindItem)(int);
int (*AddNewItem)(void *instance, int typeId, int num);
void (*old_BuyShopItem)(void *instance, short index, int typeId, short num);
void BuyShopItem(void *instance, short index, int typeId, short num){
    if (instance != NULL && addItem){
        int* ret = FindItem(itemId);
        if(itemId != -1 && ret != 0){
            void* GSItemMgr = *(void**)((uint64_t) instance + 0xA4);
            AddNewItem(GSItemMgr,itemId,1);
        }
    }
    old_BuyShopItem(instance, index, typeId, num);
}
int (*getForceId)(void* instance);
void (*old_AddExp) (void*,int);
void AddExp(void* instance,int exp){
    if(instance != NULL && modWuXing){
        int forceId = getForceId(instance);
        if(forceId == 1){
            void* BaseAtt = *(void**)((uint64_t)instance + 0x14);
            int* wuxing = (int*)((uint64_t)BaseAtt + 0x38);
            *wuxing = 10000;
        }
    }
    old_AddExp(instance,exp);
};
void (*old_AddOneAge)(void*);
void AddOneAge(void* instance){
    int forceId = getForceId(instance);
    if(instance != NULL && forceId == 1 && keepYoung){
        int* age = (int*)((uint64_t)instance + 0x40);
        int* age_key = (int*)((uint64_t)instance + 0x3C);
        *age_key = 0;
        *age = 20;
    }else{
        old_AddOneAge(instance);
    }
}
void (*old_UpdateSave)(void*);
void UpdateSave(void* instance){
    if(instance != NULL && stopSave){

    }else{
        old_UpdateSave(instance);
    }
}
bool cheatAgeDetection(void* instance){
    if(instance != NULL){
        return false;
    }
    return false;

}

//void (*old_updateProduceReward)(void* , void* );
//void updateProduceReward(void* GSBuild, void* GSProduceTeam){
//    if(GSBuild != NULL && rewardEach){
//        *(int*)((uint64_t) GSProduceTeam + 0x14) = 0;
//    }
//   old_updateProduceReward(GSBuild,GSProduceTeam);
//}
void (*old_addProduceExp)(void*,int ,int exp,int ,bool);
void addProduceExp(void* GSActor,int professionId,int exp,int produceId,bool success){
    if(GSActor != NULL && addProduce){
        old_addProduceExp(GSActor,professionId,10000,produceId,success);
    }else{
        old_addProduceExp(GSActor,professionId,exp,produceId,success);
    }
}
void (*old_RemoveItem)(void* ,void*,int,bool);
void RemoveItem(void* instance,void* GSItem,int num,bool isEquip){
    if(instance != NULL && notRemoveItem){

    }else{
        old_RemoveItem(instance,GSItem,num,isEquip);
    }

}
void (*old_RemoveItemId)(void*,int,int);
void RemoveItemTypeId(void* instance, int typeId, int num){
    if(instance != NULL && notRemoveItem){

    }else{
        old_RemoveItemId(instance,typeId,num);
    }
}
int (*old_CanUseItem)(void*,void* ,void*);
int CanUseItem(void* instance, void* GSItem, void* GSActor){
    if(instance != NULL && forceUseItem){
        return 0;
    }else{
        return old_CanUseItem(instance, GSItem, GSActor);
    }
}
bool (*old_CheckProduceCost)(void*, void*, void*);
bool CheckProduceCost(void* instance,void* GSProduceTeam,void* MakingItemInfo){
    if(instance != NULL && forceProduce){
        return true;
    }else{
        return old_CheckProduceCost(instance,GSProduceTeam,MakingItemInfo);
    }
}
int hadDetecedCheat(void* instance){
    if(instance != NULL){
       return 0;
    }
}
void OnCheatDetected(void* instance, int times){

}
// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));
    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);
#if defined(__aarch64__)
#else
    LOGD(OBFUSCATE("else_structure"));

    HOOK_LIB_NO_ORIG("libil2cpp.so", "0xb5988c", cheatAgeDetection);//
    HOOK_LIB_NO_ORIG("libil2cpp.so", "0xaae440", hadDetecedCheat);//
    HOOK_LIB_NO_ORIG("libil2cpp.so", "0xab1984", OnCheatDetected);
    HOOK_LIB("libil2cpp.so", "0xb4a9d0", addProduceExp,old_addProduceExp);
    HOOK_LIB("libil2cpp.so", "0xb42d1c", AddExp,old_AddExp);
    HOOK_LIB("libil2cpp.so", "0x961f80", BuyShopItem, old_BuyShopItem);
    HOOK_LIB("libil2cpp.so", "0xb59804", AddOneAge, old_AddOneAge);//
    HOOK_LIB("libil2cpp.so", "0xa91f28", UpdateSave, old_UpdateSave);
    HOOK_LIB("libil2cpp.so", "0xa9d628", RemoveItem,old_RemoveItem);
    HOOK_LIB("libil2cpp.so", "0xa97efc", RemoveItemTypeId,old_RemoveItemId);
    HOOK_LIB("libil2cpp.so", "0xa9e088", CanUseItem,old_CanUseItem);
    HOOK_LIB("libil2cpp.so", "0x946e40", CheckProduceCost,old_CheckProduceCost);

//    HOOK_LIB("libil2cpp.so", "0x2923A0", updateProduceReward,old_updateProduceReward);
    FindItem = (int*(*)(int))getAbsoluteAddress("libil2cpp.so", 0xb00ca4);
    AddNewItem = (int(*)(void*, int ,int))getAbsoluteAddress("libil2cpp.so", 0xa9d2a4);
    getForceId = (int (*)(void *))getAbsoluteAddress("libil2cpp.so", 0xb43844);

#endif
    return NULL;
}

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically countId for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;
    const char *features[] = {
            OBFUSCATE("Category_修仙家族模拟器MOD菜单"), //Not counted
            OBFUSCATE("Category_星联版"),
            OBFUSCATE("3_Toggle_根据代码添加物品"),
            OBFUSCATE("4_InputValue_请输入物品代码"),
            OBFUSCATE("100_Toggle_True_天赋全选，看广告加大量功德"),
            OBFUSCATE("1001_Toggle_10000悟性"),
            OBFUSCATE("500_Toggle_物品不减"),
            OBFUSCATE("805_Toggle_快速探索"),
            OBFUSCATE("804_Toggle_无需物品制造"),
            OBFUSCATE("900_Toggle_快速制作"),
            OBFUSCATE("600_Toggle_副职经验+10000"),
            OBFUSCATE("802_Toggle_修为丹药千倍效果"),
            OBFUSCATE("Category_OGM"),
            OBFUSCATE("Category_请大家支持正版"),
            OBFUSCATE("103_Toggle_商店购买不减"),
            OBFUSCATE("400_Toggle_祝福大家永远20岁"),
            OBFUSCATE("800_Toggle_无条件使用物品"),
            OBFUSCATE("890_Toggle_关闭自动存档"),

    };
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));
    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
             jint featNum, jstring featName, jint value,
             jboolean boolean, jstring str) {
    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");
    if(featNum == 4){
        itemId = value;
    }

    switch (featNum) {
        case 3:
            itemId = value;
            addItem = boolean;
            break;
        case 400:
            keepYoung = boolean;
            break;
        case 1001:
            modWuXing = boolean;
            break;
        case 890:
            stopSave = boolean;
            break;
        case 500:
            notRemoveItem = boolean;
            break;
        case 600:
            addProduce = boolean;
            break;
        case 800:
            forceUseItem = boolean;
            //无条件使用物品public int CanUseItem(GSItem item, GSActor p)   此处只需要修改地址即可
//            PATCH_LIB_SWITCH("libil2cpp.so", "0xb70ff8", "00 00 A0 E3 1E FF 2F E1", boolean);
            break;

        case 804:
            forceProduce = boolean;
            //无需物品制造    只需修改地址
            //***public bool CheckProduceCost(GSProduceTeam data, MakingItemInfo proto)
            break;
            //看广告加大量功德
        case 802:
            //修为丹药千倍效果 只需修改地址calcattgroup
            PATCH_LIB_SWITCH("libil2cpp.so", "0xB4270C", "FA 0F A0 E3", boolean);
            break;
        case 100:
            //public int GetTalentsCount()
            PATCH_LIB_SWITCH("libil2cpp.so", "0x8b4af8", "05 00 A0 E3 1E FF 2F E1", boolean);
            //CanAddTalent()
            PATCH_LIB_SWITCH("libil2cpp.so", "0xb5a450", "01 00 A0 E3 1E FF 2F E1", boolean);

            //GetSelectedTalentsNeedMerits
            PATCH_LIB_SWITCH("libil2cpp.so", "0x984308", "01 00 A0 E3 1E FF 2F E1", boolean);

            //不跳转 OnAddSelectTalent nop掉
            PATCH_LIB_SWITCH("libil2cpp.so", "0x983E8C", "00 F0 20 E3", boolean);
            //直接添加进list
            PATCH_LIB_SWITCH("libil2cpp.so", "0x983EAC", "1E 00 00 EA", boolean);
            //看广告加大量功德
            PATCH_LIB_SWITCH("libil2cpp.so", "0x9A4BE0", "10 07 02 E3", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x9846DC", "10 07 02 E3", boolean);
            //9846DC
            break;
        case 805:
            //快速探索  只需修改地址updateexploreresult，不跳转固定值
            // il2cpp:003390D0 00 00 55 E1                       CMP             R5, R0
            //il2cpp:003390D4 44 01 00 CA                       BGT             loc_3395EC
            PATCH_LIB_SWITCH("libil2cpp.so", "0x971898", "00 F0 20 E3", boolean);
            break;
        case 900:
            //GS_GSBuild__UpdateProduceReward
            //每旬收获物品    此处只需要修改地址即可
            PATCH_LIB_SWITCH("libil2cpp.so", "0x946CA0", "00 F0 20 E3", boolean);
            break;

        case 103:
            //购买商店物品
            //调用DecreaseMoney nop掉
            PATCH_LIB_SWITCH("libil2cpp.so", "0x96211C", "00 F0 20 E3", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x962150", "00 F0 20 E3", boolean);
            break;
            //0x80d7c8





    }
}
__attribute__((constructor))

void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}
int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}
extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}

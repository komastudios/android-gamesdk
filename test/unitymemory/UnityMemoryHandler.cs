using System;
using UnityEngine;

// This class demonstrates how the Memory Advice API can be used by a Unity application.
public class UnityMemoryHandler : MonoBehaviour
{
    private AndroidJavaObject _memoryAdvisor;

    void Start()
    {
        AndroidJavaClass unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
        AndroidJavaObject activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
        AndroidJavaObject context = activity.Call<AndroidJavaObject>("getApplicationContext");
        _memoryAdvisor = new AndroidJavaObject(
            "com.google.android.apps.internal.games.memoryadvice.MemoryAdvisor", context);

        JSONObject deviceInfo = ConvertJsonObject(_memoryAdvisor.Call<AndroidJavaObject>(
            "getDeviceInfo", context));
        // deviceInfo is a C# object that can be inspected, or logged.
    }

    void Update()
    {
        // This example calls the advisor every frame. Because of the cost (~5 to 20 milliseconds)
        // clients may choose to poll less frequently.

        AndroidJavaObject adviceNative = _memoryAdvisor.Call<AndroidJavaObject>("getAdvice");
        // Detailed memory information is obtained as a native Java object.
        // This can be passed back into the library to get simple memory advice. Or, use
        // ConvertJsonObject(adviceNative) to get a C# object that can be inspected for more detail.

        if (_memoryAdvisor.CallStatic<Boolean>("anyRedWarnings", adviceNative))
        {
            // Memory should be freed.
        }

        if (!_memoryAdvisor.CallStatic<Boolean>("anyWarnings", adviceNative))
        {
            // It is OK to make significant allocations.
        }

        long availabilityEstimate =
            _memoryAdvisor.CallStatic<long>("availabilityEstimate", adviceNative);
        // availabilityEstimate contains an estimate in bytes of memory that could be safely
        // allocated.
    }

    // Helper method to convert a native Java JSON object into a C# object that can be inspected.
    // Requires JSONObject by Defective Studios.
    // https://assetstore.unity.com/packages/tools/input-management/json-object-710
    private static JSONObject ConvertJsonObject(AndroidJavaObject androidJavaObject)
    {
        return JSONObject.Create(androidJavaObject.Call<string>("toString"));
    }
}
using System;
using System.Collections.Generic;
using UnityEngine;
using Firebase.TestLab;
using System.IO;

public class FirebaseMemoryTest : MonoBehaviour
{
    private TestLabManager _testLabManager;
    private AndroidJavaObject _info;
    private JSONObject _flattened;
    private long _delay;
    private long _startTime;

    // Start is called before the first frame update
    void Start()
    {
        _startTime = DateTimeOffset.Now.ToUnixTimeMilliseconds();
        _testLabManager = TestLabManager.Instantiate();

        using (StreamReader r = new StreamReader(File.OpenRead("/sdcard/params.json")))
        {
            JSONObject params1 = new JSONObject(r.ReadToEnd());
            JSONObject first = JSONObject.obj;

            using (AndroidJavaClass helper =
                new AndroidJavaClass("com.google.android.apps.internal.games.helperlibrary.Helper"))
            {
                first["build"] =
                    ConvertJsonObject(helper.CallStatic<AndroidJavaObject>("getBuild"));
            }

            first["params"] = params1;
            _testLabManager.LogToResults(first.Print() + Environment.NewLine);
            _info =
                new AndroidJavaObject("com.google.android.apps.internal.games.helperlibrary.Info");

            _flattened = FlattenParams(params1);
            JSONObject jsonObject = _flattened.GetField("delay");
            if (jsonObject == null)
            {
                ApplySettings();
            }
            else
            {
                _delay = jsonObject.i;
            }
        }
    }

    private static JSONObject ConvertJsonObject(AndroidJavaObject androidJavaObject)
    {
        return JSONObject.Create(androidJavaObject.Call<string>("toString"));
    }

    /**
     * <summary>
     * Activate (or disable) the specified game object and all of its children.
     * When disabling, the objects contained in <paramref name="keepActivated"/>
     * won't be disabled.
     * </summary>
     */
    bool SetActiveRecursively(GameObject root, bool enable, ISet<GameObject> keepActivated)
    {
        bool containsActiveChildren = false;
        foreach (Transform childTransform in root.transform)
        {
            var gameObject = childTransform.gameObject;
            if (!gameObject)
            {
                continue;
            }

            if (keepActivated.Contains(gameObject))
            {
                containsActiveChildren = true;
            }
            else
            {
                containsActiveChildren |= SetActiveRecursively(gameObject, enable, keepActivated);
            }
        }

        root.SetActive(enable || containsActiveChildren);
        return enable || containsActiveChildren;
    }

    // Update is called once per frame
    void Update()
    {
        if (DateTimeOffset.Now.ToUnixTimeMilliseconds() - _startTime > _delay)
        {
            ApplySettings();
        }

#if UNITY_ANDROID
        AndroidJavaClass unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
        AndroidJavaObject activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
        AndroidJavaObject context = activity.Call<AndroidJavaObject>("getApplicationContext");
        JSONObject report = 
            ConvertJsonObject(_info.Call<AndroidJavaObject>("getMemoryMetrics", context));
        _testLabManager.LogToResults(report.Print() + Environment.NewLine);
#endif
    }

    private void ApplySettings()
    {
        _delay = long.MaxValue;
        if (_flattened.HasField("quality"))
        {
            QualitySettings.SetQualityLevel((int) _flattened.GetField("quality").i);
        }

        if (_flattened.HasField("destroy"))
        {
            foreach (JSONObject obj in _flattened.GetField("destroy").list)
            {
                Destroy(GameObject.Find(obj.str));
            }
        }

        if (_flattened.HasField("disable"))
        {
            foreach (JSONObject obj in _flattened.GetField("disable").list)
            {
                SetActiveRecursively(GameObject.Find(obj.str), false, new HashSet<GameObject>());
            }
        }
    }

    private static JSONObject FlattenParams(JSONObject params1)
    {
        JSONObject p = JSONObject.obj;
        JSONObject coordinates = params1["coordinates"];
        JSONObject tests = params1["tests"];

        for (int coordinateNumber = 0; coordinateNumber != coordinates.Count; coordinateNumber++)
        {
            JSONObject jsonArray = tests[coordinateNumber];
            JSONObject jsonObject = jsonArray[(int) coordinates[coordinateNumber].i];
            foreach (string key in jsonObject.keys)
            {
                p[key] = jsonObject[key];
            }
        }
        return p;
    }
}
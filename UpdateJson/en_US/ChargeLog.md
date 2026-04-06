# Version Code 88

## Update
- Added extensive error handling and feedback optimization
- Button in Root Manager changed to launch ClearBox App, falls back to one-click optimize and clean if launch fails

## Add
- StopCached now supports multiple external storage handling
- App now supports adaptive icons, upgraded target API version to 36 (Android 16), UI optimization
- Added cleaning rules：
```
com.larus.nova          |  Doubao（zh-CN）
com.tencent.wemeet.app  |  Tencent Meeting
org.mozilla.firefox     |  Firefox
```

## Remove
- Removed i686 support. If you need it, it is recommended to build from source

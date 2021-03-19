BIN	= $(DESTDIR)/opt/Unity
UNITY_DESTINATION = $(BIN)/Editor
MONODEVELOP_DESTINATION = $(BIN)/MonoDevelop
MENUITEM_PATH = $(DESTDIR)/usr/share/applications
UNITY_ICON_PATH = $(DESTDIR)/usr/share/icons/hicolor/256x256/apps
MONODEVELOP_ICON_PATH = $(DESTDIR)/usr/share/icons/hicolor/48x48/apps

install:
	env
	mkdir -p $(UNITY_DESTINATION)
	mkdir -p $(MONODEVELOP_DESTINATION)
	cp -R $(UNITY_SOURCE)/* $(UNITY_DESTINATION)
	cp -R $(MONODEVELOP_SOURCE)/* $(MONODEVELOP_DESTINATION)
	mkdir -p $(MENUITEM_PATH)
	cp  unity-editor.desktop $(MENUITEM_PATH)
	cp  unity-monodevelop.desktop $(MENUITEM_PATH)
	mkdir -p $(UNITY_ICON_PATH)
	cp -R $(UNITY_SOURCE)/Data/Resources/LargeUnityIcon.png $(UNITY_ICON_PATH)/unity-editor-icon.png
	mkdir -p $(MONODEVELOP_ICON_PATH)
	# Need new icon
	# cp -R $(MONODEVELOP_SOURCE)/share/icons/hicolor/48x48/apps/monodevelop.png $(MONODEVELOP_ICON_PATH)/unity-monodevelop.png

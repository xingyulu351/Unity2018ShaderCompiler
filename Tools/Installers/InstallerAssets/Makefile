# combine low and hi res images into tiff, to be able to set dmg background
# more info:
# https://developer.apple.com/library/mac/documentation/GraphicsAnimation/Conceptual/HighResolutionOSX/Optimizing/Optimizing.html

all:
	tiffutil -cathidpicheck disk-image-bg.png disk-image-bg@2x.png -out disk-image-bg.tiff
	tiffutil -cathidpicheck background.png background@2x.png -out background.tiff

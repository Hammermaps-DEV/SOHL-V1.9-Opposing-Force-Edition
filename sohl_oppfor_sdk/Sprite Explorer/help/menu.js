var strActiveItem=null;
var arPreloadedImgs=new Array(32);
var arPreloadedImgNames=new Array(32);

var bImagesLoaded=false;

function showmsg(strMsg)
{
	alert(strMsg);
	window.status=strMsg;
}

function preloadImages()
{
	var numargs = preloadImages.arguments.length;


   for (i = 0; i < numargs; i++)
	{
		arPreloadedImgs[i*3]=new Image;
		arPreloadedImgs[i*3].src=document.images[preloadImages.arguments[i]].inact_src;

		arPreloadedImgs[i*3+1]=new Image;
		arPreloadedImgs[i*3+1].src=document.images[preloadImages.arguments[i]].hover_src;

		arPreloadedImgs[i*3+2]=new Image;
		arPreloadedImgs[i*3+2].src=document.images[preloadImages.arguments[i]].act_src;

		arPreloadedImgNames[i]=preloadImages.arguments[i];
	}
	bImagesLoaded=true;
}

function getPreloadedImageIndex(strImgName)
{
	var i=0;

	for(i=0;i<32;i++)
	{
		if(arPreloadedImgNames[i]==strImgName) return i;
	}

	return -1;
}

function lightMenuItem(strItemName, bChangeTopMenu, strTopMenuLoc, strTopMenuItemName)
{
	var idx=0;

	if(bImagesLoaded && document.images[strItemName]!=null && strActiveItem!=strItemName)
	{
		idx=getPreloadedImageIndex(strItemName);
		if(idx>=0) document.images[strItemName].src = arPreloadedImgs[idx*3+1].src;
		else document.images[strItemName].src = document.images[strItemName].hover_src;
	}

}

function dimMenuItem(strItemName)
{
	var idx=0;

	if(bImagesLoaded && document.images[strItemName]!=null && strActiveItem!=strItemName)
	{
		idx=getPreloadedImageIndex(strItemName);
		if(idx>=0) document.images[strItemName].src = arPreloadedImgs[idx*3].src;
		else document.images[strItemName].src = document.images[strItemName].inact_src;
	}

}

function activateMenuItem(strItemName)
{
	var idx=0;

	if(bImagesLoaded && document.images[strItemName]!=null && strActiveItem!=strItemName)
	{
		if(strActiveItem!=null)
		{
			idx=getPreloadedImageIndex(strActiveItem);
			if(idx>=0) document.images[strActiveItem].src = arPreloadedImgs[idx*3].src;
			else document.images[strActiveItem].src = document.images[strActiveItem].inact_src;
		}

		idx=getPreloadedImageIndex(strItemName);
		if(idx>=0) document.images[strItemName].src = arPreloadedImgs[idx*3+2].src;
		else document.images[strItemName].src = document.images[strItemName].act_src;

		strActiveItem=strItemName;
	}

}

function deactivateMenu()
{
	if(bImagesLoaded && document[strActiveItem]!=null) {
		if(strActiveItem!=null) document[strActiveItem].src = document[strActiveItem].inact_src;
		strActiveItem=null;
	}
}
/*	Get data from global variable spectrum_data and utilities to manupulate----
 * 	the data-------------------------------------------------------------------*/
SpectrumParameters = function(peakdata) {
  /*assign the mono m/z value when clicked on mono m/z*/
  this.minMz ;
  this.maxMz ;
  this.centerMz ;
  this.xScale = 0;
  this.yScale;

  this.dataMinMz ;
  this.dataMaxMz ;
  this.dataMaxInte ;
  this.dataMinInte;
  this.maxInte;

  this.svgWidth = 910;
  this.svgHeight = 220;
	/*	Set padding values to the svg------------------*/
	this.padding = {left:70, right:20, head:10, bottom:50};
	this.specWidth = this.svgWidth - this.padding.left - this.padding.right;
	this.specHeight = this.svgHeight - this.padding.head - this.padding.bottom;

  this.specScale = 1.0;
  this.specX = 0;


  /*	Colors for the envelope circles	*/
  this.spectrumColorArray = ["red","orange","blue","green"]
  /*	Radius factor and setting min and max radius	*/
  this.mzRadius = 0.05;
  this.minRadius = 2;
  this.maxRadius = 5;
	
  this.tickWidthList = [10000,8000,6000,5000,4000,3000,2000,1000,800,700,600,500,450,400,350,300,250,200,150,100,50,20,10,5,3,2,1,0.5,0.2,0.1,0.05,0.01,0.005,0.001,0.0005,0.0001,0.00005,0.00001,0.000005,0.000001];
  this.tickHeightList = [50,40,30,25,20,15,10,5,3,2,1,0.5,0.2,0.1,0.05,0.01,0.005,0.001];
  this.onClickMassAdjacentRange = 3;
  this.mouseOverPadding = {head:20,middle:14};
	
/* number of ticks on x and y axis*/
	this.xTicks = 10;
  this.yTicks = 5 ;
  //Height/size of the tick
	this.ticklength = 7 ;

  this.initScale = function(currminMz, currmaxMz, dataMaxInte,dataMinInte,minMzData,maxMzData,currentMaxIntensity) {
    this.dataMinMz = minMzData;
    this.dataMaxMz = maxMzData + (0.10 * maxMzData);
    this.minMz = currminMz;
    this.maxMz = currmaxMz;
    if(currminMz == minMzData && currmaxMz == maxMzData)
    {
      this.minMz = 0;
      this.maxMz = this.maxMz + (0.10*this.maxMz);
    }
    this.centerMz = (this.minMz + this.maxMz)/2.0;
    this.xScale = this.specWidth/(this.maxMz - this.minMz);

    /* add 1/4th of max intensity to keep the max point at 3/4 of the y axis*/
    this.dataMaxInte = dataMaxInte + 0.25*dataMaxInte;
    this.dataMinInte = dataMinInte ;
      //this.maxInte = dataMaxInte;
    this.maxInte = currentMaxIntensity;
    /* add 1/4th of max intensity to keep the max point at 3/4 of the y axis*/
    //dataMaxInte = dataMaxInte + 0.25*dataMaxInte ;
    currentMaxIntensity = currentMaxIntensity + 0.25*currentMaxIntensity ;
      this.yScale = this.specHeight/currentMaxIntensity;
  }
  this.setColorToEnvelops = function(envelopes){
      envelopes.sort(function(x,y){
        return d3.ascending(x.env_peaks[0].mz, y.env_peaks[0].mz);
      })
      let colorListsize = this.spectrumColorArray.length;
      let i = envelopes.length ;
      while(i--)
      {
        envelopes[i].color = this.spectrumColorArray[i%colorListsize];
      }
      return envelopes;
  }
  this.getPeakXPos = function (mz) {
    let peakX = (mz - this.minMz) * this.xScale + this.padding.left;
    return peakX;
  }

  this.getPeakYPos = function (intensity) {
    //peakY = intensity * this.yScale + this.padding.bottom;
    let peakY = this.svgHeight - intensity * this.yScale - this.padding.bottom;
    return peakY;
  }

  this.getCircleSize = function() {
    radius = this.mzRadius * this.xScale;
    if (radius < this.minRadius) {
      radius = this.minRadius;
    }
    if (radius > this.maxRadius) {
      radius = this.maxRadius;
    }
    return radius;
  }
	this.getTickWidthList = function(){
    let tempDiff = this.maxMz - this.minMz;
		let i = this.dataMaxMz/this.xTicks * 2 ;
		let maxTickWidth = this.tickWidthList[0]
		if(i > maxTickWidth)
		{
			this.tickWidthList.push(Math.ceil(i / 50)*50);
			while(i > maxTickWidth){
				i= i/2 ; 
				// rounding the values to nearest 50
				let val = Math.ceil(i / 50)*50 ;
				this.tickWidthList.push(val);
			}
			this.tickWidthList = [... new Set(this.tickWidthList)];
			this.tickWidthList.sort(function(a, b){return b-a});
		}
		return this.tickWidthList ;
  }
  this.getTickWidth = function(){
  let tempDiff = this.maxMz - this.minMz;
  let tickWidth = parseInt(this.tickWidthList[0]) ;
	for(let i = 0; i < this.tickWidthList.length; i++)
	{
		if(tempDiff/this.xTicks <= parseFloat(this.tickWidthList[i]) && tempDiff/this.xTicks > parseFloat(this.tickWidthList[i+1]))
		{
			tickWidth = parseFloat(this.tickWidthList[i]);
			break ;
		}
  }
	  return 	tickWidth ;
  }
  this.getTickHeight = function(){
    let tickheight = parseInt(this.tickHeightList[0]) ;
    let maxIntPercent = this.maxInte/this.dataMaxInte * 100;
		for(let i = 0; i < this.tickHeightList.length; i++)
		{
			if(maxIntPercent/this.yTicks <= parseFloat(this.tickHeightList[i]) && maxIntPercent/this.yTicks > parseFloat(this.tickHeightList[i+1]))
			{
				tickheight = parseFloat(this.tickHeightList[i]);
				break ;
			}
    }
	  return tickheight ;
  }

  this.xZoom = function (mouseSvgX, ratio) {
   if ((ratio > 1.0) || ((this.maxMz - this.minMz) < this.dataMaxMz) ) {
      let mouseSpecX = mouseSvgX - this.padding.left;
      this.centerMz =  mouseSpecX/this.xScale + this.minMz;
      /*self is a global variable of datasource object containing all the data needed to use when zoomed*/
      this.xScale = this.xScale * ratio ; 
      this.minMz = this.centerMz - mouseSpecX / this.xScale; 
      this.maxMz = this.centerMz + (this.specWidth - mouseSpecX) / this.xScale;
    }
  }

  this.yZoom = function (ratio) {
    //Reducing zoom factor to smoothenup and remove gliches
    if(ratio > 1 ) ratio = 1.4;
    else if(ratio < 1) ratio = 0.9;
    if ((ratio > 1.0 && this.maxInte >= this.dataMinInte ) || (ratio < 1.0 && this.maxInte <= this.dataMaxInte)) {
      this.yScale = this.yScale * ratio;
      this.maxInte = this.specHeight / this.yScale;
    }
  }

  this.zoom = function(mouseSvgX, mouseSvgY, ratio) {
    if(ratio > 1 ) ratio = 1.4;
    else if(ratio < 1) ratio = 0.9;
    if (mouseSvgY > this.svgHeight - this.padding.bottom) {
      this.xZoom(mouseSvgX, ratio);
    }
    else {
      this.yZoom(ratio);
    }
  }
  this.drag = function(distX) {
    let mzDist = distX / this.xScale;
    this.minMz = this.minMz - mzDist; 
    this.maxMz = this.maxMz - mzDist;
    this.centerMz = this.centerMz - mzDist;
  }
}

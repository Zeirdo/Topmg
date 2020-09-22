var currentMinXTickVal = 0 ;
var currentMaxXTickVal = 0 ;
SpectrumGraph = function(svgId,spectrumParameters,peakData){
	this.svg = d3.select("body").select(svgId);

  this.zoomed = function () {
    let transform = d3.event.transform;
    let distance = transform.x - spectrumParameters.specX;
	let ratio = transform.k / spectrumParameters.specScale;
    spectrumParameters.specX = transform.x;
    spectrumParameters.specScale = transform.k;
    let mousePos = d3.mouse(this);
    if (ratio == 1) 
    {
      	spectrumParameters.drag(distance);
    }
    else 
    {
		spectrumParameters.zoom(mousePos[0], mousePos[1], ratio);
    }
	drawSpectrum(svgId,spectrumParameters,peakData);
  }

  this.zoom = d3.zoom()
    .on("zoom", this.zoomed);
	this.svg.attr("viewBox", "0 0 "+spectrumParameters.svgWidth+" "+spectrumParameters.svgHeight)
					.attr("width", "100%")
					.attr("height", "100%")
					.call(this.zoom);
	this.svg.call(this.zoom.transform, d3.zoomIdentity);
												
  drawSpectrum(svgId,spectrumParameters, peakData);
}

drawTicks = function(svg,spectrumParameters,spectrumgraph){
	
	this.addXTicks = svg.append("g").attr("id","ticks");
	
	for(let i=0; i <= spectrumParameters.xTicks ; i++)
	{
		let tickWidth = spectrumParameters.getTickWidth();
		if(tickWidth < 1 && tickWidth != 0)
		{
			tickWidth = (i*tickWidth + spectrumParameters.minMz) - parseFloat((i*tickWidth + spectrumParameters.minMz)%tickWidth) ;
		}
		else if(tickWidth != 0)
		{
			tickWidth = i*tickWidth + spectrumParameters.minMz - (i*tickWidth + spectrumParameters.minMz)%tickWidth ;
		}
		x = spectrumParameters.getPeakXPos(tickWidth);
		if(x >= spectrumParameters.padding.left - 0.2 && 
				x <= (spectrumParameters.svgWidth - spectrumParameters.padding.right))
		{
			this.addXTicks.append("line")
							.attr("x1",x)
							.attr("y1",spectrumParameters.svgHeight -spectrumParameters.padding.bottom)
							.attr("x2",x)
							.attr("y2",spectrumParameters.svgHeight -spectrumParameters.padding.bottom + spectrumParameters.ticklength)
							.attr("stroke","black")
							.attr("stroke-width","1")
		}
	}
	this.addYTicks = svg.append("g").attr("id","ticks");
	
	for(let i=0; i <= spectrumParameters.yTicks ; i++)
	{
		let tickHeight = spectrumParameters.getTickHeight();
		tickHeight = i*tickHeight * spectrumParameters.dataMaxInte /100;
		tickHeight = parseFloat(spectrumParameters.getPeakYPos(tickHeight)) ;
		let y =  tickHeight;
		if(y < spectrumParameters.padding.head )
		{
			y =  -1000;
		}
		if(!isNaN(y))
		{
			this.addYTicks.append("line")
						.attr("x1",spectrumParameters.padding.left)
						.attr("y1",y)
						.attr("x2",spectrumParameters.padding.left - spectrumParameters.ticklength)
						.attr("y2",y)
						.attr("stroke","black")
						.attr("stroke-width","1")
		}
	}
}

drawAxis = function(svg,spectrumParameters,spectrumgraph){
	this.xAxis = svg.append("g").attr("id", "axis").append("line")
					.attr("x1",spectrumParameters.padding.left)
					.attr("y1",spectrumParameters.svgHeight -spectrumParameters.padding.bottom)
					.attr("x2",spectrumParameters.specWidth+spectrumParameters.padding.left)
					.attr("y2",spectrumParameters.svgHeight -spectrumParameters.padding.bottom)
					.attr("stroke","black")
					.attr("stroke-width","2")
	
	this.yAxis = svg.append("g").attr("id", "axis").append("line")
					.attr("x1",spectrumParameters.padding.left)
					.attr("y1",spectrumParameters.padding.head)
					.attr("x2",spectrumParameters.padding.left)
					.attr("y2",spectrumParameters.svgHeight -spectrumParameters.padding.bottom)
					.attr("stroke","black")
					.attr("stroke-width","2")
}

addDatatoAxis = function(svg,spectrumParameters){
	//let currentMinPeakVal;
	//let currentMaxPeakVal;
	let maxMz = spectrumParameters.maxMz;
	let minMz = spectrumParameters.minMz ;
	let maxInte = spectrumParameters.dataMaxInte ;
	this.xAxisData = svg.append("g")
						.attr("id", "axisPoints");
						
	for(let i = 0 ; i <=spectrumParameters.xTicks ; i++)
	{
		let tickWidth = spectrumParameters.getTickWidth();
		if(tickWidth < 1 && tickWidth != 0)
		{
			tickWidth = i*tickWidth + spectrumParameters.minMz - parseFloat((i*tickWidth + spectrumParameters.minMz)%tickWidth) ;
		}
		else if(tickWidth != 0)
		{
			tickWidth = i*tickWidth + spectrumParameters.minMz - (i*tickWidth + spectrumParameters.minMz)%tickWidth ;
		}
		x = spectrumParameters.getPeakXPos(tickWidth);
		let l_tickWidth = tickWidth;
		if(x >= spectrumParameters.padding.left - 0.2 && 
				x <= (spectrumParameters.svgWidth - spectrumParameters.padding.right))
		{
			this.xAxisData.append("text").attr("id","xtext").attr("x",x)
						.attr("y",spectrumParameters.svgHeight - (spectrumParameters.padding.bottom/1.6))
						.attr("text-anchor","middle")
						.text(function(){
							if(maxMz - minMz <=0.0001) return parseFloat(l_tickWidth).toFixed(6);
							else if(maxMz - minMz <=0.001) return parseFloat(l_tickWidth).toFixed(5);
							else if(maxMz - minMz <=0.01) return parseFloat(l_tickWidth).toFixed(4);
							else if(maxMz - minMz <=0.1) return parseFloat(l_tickWidth).toFixed(3);
							else if(maxMz - minMz <=1) return parseFloat(l_tickWidth).toFixed(2);
							else if(maxMz - minMz <= 3) return parseFloat(l_tickWidth).toFixed(2)
							else if(maxMz - minMz <= 5) return parseFloat(l_tickWidth).toFixed(1)
							return parseInt(l_tickWidth)
						})
						.style("font-size","14px")
		}
		if(i == 0) currentMinXTickVal = l_tickWidth;
		else if(i == spectrumParameters.xTicks) currentMaxXTickVal = l_tickWidth ;
		
	}
	this.yAxisData = svg.append("g")
							.attr("id", "axisPoints");
	for(let i = 0 ; i <= spectrumParameters.yTicks ; i++)
	{
		let tickHeight = 0;
		tickHeight = spectrumParameters.getTickHeight();
		let data = i*tickHeight ;
		if(data <= 1 && data != 0) data = data.toFixed(1);
		tickHeight = i*tickHeight * spectrumParameters.dataMaxInte /100;
		tickHeight = parseFloat(spectrumParameters.getPeakYPos(tickHeight)) ;
		let y =  tickHeight;
		if(y < spectrumParameters.padding.head ) y =  -1000;
		if(!isNaN(y))
		{
			this.xAxisData.append("text").attr("class","ytext").attr("x",spectrumParameters.padding.left - spectrumParameters.ticklength)
						.attr("y",y)
						.attr("text-anchor","end")
						.attr("alignment-baseline","middle")
						.text(data + "%")
						.style("font-size","14px")
		}
	}
	//return [currentMinPeakVal,currentMaxPeakVal];
}
drawPeaks = function(svg,spectrumParameters,peakdata){
	let peaks = svg.append("g")
    				.attr("id", "peaks");
      let max = 0 ;
	  var len = peakdata.peak_list.length;
	  for(let i =0;i<len;i++)
	  {
		let peak = peakdata.peak_list[i];
		if(peak.mz >= spectrumParameters.minMz && peak.mz <= spectrumParameters.maxMz)
		{
			peaks.append("line")
		  .attr("x1",function(d,i){
			  return spectrumParameters.getPeakXPos(peak.mz);
			  })
		  .attr("y1",function(d,i){
				let y = spectrumParameters.getPeakYPos(peak.intensity);
				if(y<=spectrumParameters.padding.head) return spectrumParameters.padding.head ;
				else return y ;
			  })
		  .attr("x2",function(d,i){
			  	return spectrumParameters.getPeakXPos(peak.mz);
			  })
		  .attr("y2",spectrumParameters.svgHeight - spectrumParameters.padding.bottom )
		  .attr("stroke","black")
		  .attr("stroke-width","2")
		  .on("mouseover",function(d,i){
					onMouseOverPeak(this,svg,peak,spectrumParameters);
				})
			.on("mouseout",function(d,i){
				onPeakMouseOut(this);
			});
			if(max == 200)
			{
				break;
			}
			max = max+1;
		}
	  }
}
addCircles = function(svg,spectrumParameters,peakData){
	let circles = svg.append("g").attr("id", "circles");
	let minPercentage = 0.5;
	let maxIntensity = spectrumParameters.dataMaxInte ;
	let max = 0 ;
	peakData.envelope_list.forEach(function(envelope_list,i){
		envelope_list.env_peaks.forEach(function(env_peaks,index){
			//Show only envelopes with minimum of 0.5% 
			let percentInte = env_peaks.intensity/maxIntensity * 100 ;
			
			if(env_peaks.mz > spectrumParameters.minMz && env_peaks.mz <= spectrumParameters.maxMz && percentInte >= minPercentage)
			{
				circles.append("circle")
				.attr("id","circles")
				.attr("cx",function(d,i){
					return spectrumParameters.getPeakXPos(env_peaks.mz);
				})
				.attr("cy",function(d,i){
					let cy = spectrumParameters.getPeakYPos(env_peaks.intensity);
					if(cy < spectrumParameters.padding.head) return spectrumParameters.padding.head ;
					else return cy ;
				})
				.attr("r",function(d,i){
					return spectrumParameters.getCircleSize();
				})
				.style("fill","white")
				.style("opacity", "0.6")
				.style("stroke",envelope_list.color)
				.style("stroke-width","2")
				.on("mouseover",function(d,i){
					onMouseOverCircle(this,svg,envelope_list,spectrumParameters);
				})
				.on("mouseout",function(d,i){
					onCircleMouseOut(this);
				});
			}
		})
	})
}
addLabels = function(svg, spectrumParameters){

	svg.append("text").attr("id","label")
						.attr("transform","translate(" + (spectrumParameters.svgWidth/2) + "," + (spectrumParameters.svgHeight-spectrumParameters.padding.head) + ")")
					.attr("fill","black")
					    .attr("font-family","Helvetica Neue,Helvetica,Arial,sans-serif")
					    .attr("font-size","16px")
					    .text("m/z");
	svg.append("text").attr("id","label")
					.attr("transform", "translate("+ spectrumParameters.padding.left/3 +","+spectrumParameters.svgHeight/2+")rotate(-90)")
					.attr("fill","black")
					    .attr("font-family","Helvetica Neue,Helvetica,Arial,sans-serif")
					    .attr("font-size","16px")
					    .text("Intensity");
}
onMouseOverPeak = function(this_element,svg,peak,spectrumParameters)
{
	let x = spectrumParameters.getPeakXPos(peak.mz);
	let y = spectrumParameters.getPeakYPos(peak.intensity);
	intensity =" I:"+ parseFloat(peak.intensity).toFixed(3);
	mz = "M:"+parseFloat(peak.mz).toFixed(3);
	y = y - spectrumParameters.mouseOverPadding.head ;
	if(y<=spectrumParameters.mouseOverPadding.head)
	{
		y = spectrumParameters.mouseOverPadding.head;
	}
	d3.select(this_element).style("stroke","red")
							.style("stroke-width","2");
	
	let tooltipData = mz + "<br>" + intensity ;						
	/*	Rectangle to have flexible on click and on mouse actions	*/
	var div = d3.select("body").append("div")
							.attr("id", "MyTextMZIN")	
							.attr("class", "tooltip")	
							
	div.transition().duration(30)	
					.style("opacity", 2);
	div.html(tooltipData).style("left", (d3.event.pageX + 12)  + "px")		
				.style("top", (d3.event.pageY - 28)+ "px")
				.style("fill", "black")
				//.style("font-weight","bold");
}
onMouseOverCircle = function(this_element,svg,envelope_list,spectrumParameters)
{
	let x = parseFloat(d3.select(this_element).attr("cx"));
	let y = parseFloat(d3.select(this_element).attr("cy")) ;
	let mass = "Mass:"+envelope_list.mono_mass.toFixed(2);
	let charge = "Charge:"+ envelope_list.charge ;
	y = y - spectrumParameters.mouseOverPadding.head ;
	if(y<=spectrumParameters.mouseOverPadding.head)
	{
		y = spectrumParameters.mouseOverPadding.head;
	}
	
	let tooltipData = mass + "<br>" + charge ;						
	/*	Rectangle to have flexible on click and on mouse actions	*/
	var div = d3.select("body").append("div")
							.attr("id", "MyTextMassCharge")	
							.attr("class", "tooltip")
							
	div.transition().duration(30)	
					.style("opacity", 2);
	div.html(tooltipData).style("left", (d3.event.pageX + 12)  + "px")		
				.style("top", (d3.event.pageY - 28)+ "px")
				.style("fill", "black");
}
onPeakMouseOut = function(this_element)
{
	onMouseOut();
	d3.select(this_element).style("stroke","black");
}
onCircleMouseOut= function(){
	onMouseOut();
}
onMouseOut = function(){
	d3.selectAll("#MyTextMZIN").remove();
	d3.selectAll("#MyTextMassCharge").remove();
}
function drawSpectrum(svgId, spectrumParameters, peakData){
	// if(spectrumParameters.minMz > -500 )
	// {
		let svg = d3.select("body").select(svgId);
		svg.selectAll("#xtext").remove();
		svg.selectAll("#ticks").remove();
		svg.selectAll("#peaks").remove();
		svg.selectAll("#axisPoints").remove();
		svg.selectAll("#axis").remove();
		svg.selectAll("#circles").remove();
		svg.selectAll("#label").remove();
	/*call onMouseOut everytime to fix onHover bug adding multiple data when mouseover and zoomed up*/
	
		onMouseOut();
		drawTicks(svg, spectrumParameters, peakData);
		drawAxis(svg,spectrumParameters);
		addDatatoAxis(svg,spectrumParameters);
		drawPeaks(svg, spectrumParameters, peakData);
		addCircles(svg,spectrumParameters,peakData);
		addLabels(svg, spectrumParameters);
		SpectrumDownload.addDownloadRect(svgId, spectrumParameters);
	// }
//   addDownloadRect(svgId, spectrumParameters);
}
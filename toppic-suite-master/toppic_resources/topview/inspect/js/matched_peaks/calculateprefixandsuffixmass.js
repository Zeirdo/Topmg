calculatePrefixAndSuffixMass = function(){
	const WATER = "H2O";
	
	this.getPrefixMassList = function(sequence,massShiftList,massShift_in){
		let seqln = sequence.length;
		let emptyMassList = [] ;
		massShiftList = massShiftList.slice();
		if(seqln != 0)
		{
			let prefixMassList = new Array(seqln);
			let shiftListln = massShiftList.length;
			for(let i=0;i<seqln;i++)
			{
				// Get Calculated AminoAcidDistribution from aminoAcidDistributuion.js 
				let AcidMass = getAminoAcidDistribution(sequence[i])[0].mass;
				if(i == 0)
				{
					// Add 1 to i as the seq start from 0 but the peak id starts from 1
					if(shiftListln != 0)
					{
						AcidMass = this.addMassShift(i,massShiftList,AcidMass);
					}
					let tempObj = {position:(i+1),mass:AcidMass} ;
					prefixMassList[i] = tempObj;
				}
				else
				{
					let mass = prefixMassList[i-1].mass + AcidMass;
					if(shiftListln != 0)
					{
						mass = this.addMassShift(i,massShiftList,mass);
					}
					// Add 1 to i as the seq start from 0 but the peak id starts from 1
					let tempObj = {position:(i+1),mass:mass};
					prefixMassList[i] = tempObj;
				}
			}
			// Adding Mass Shift based on selection of Ion
			// let utilFunctionsObj = new utilFunctions();
			// let ionMassShiftVal = utilFunctionsObj.getNTerminusMassShiftVal();
			for(let j=0;j<seqln;j++)
			{
				prefixMassList[j].mass = prefixMassList[j].mass + massShift_in;
			}
			return prefixMassList;
		}
		return emptyMassList;
	}
	this.getSuffixMassList = function(sequence,massShiftList,massShift_in){
		let seqln = sequence.length;
		let emptyMassList = [];
		// As the elements starts from 0
		if(seqln != 0)
		{
			temp_seqln = seqln-1;
			let shiftListln = massShiftList.length;
			let suffixMassList = new Array(seqln);
			for(let i=temp_seqln ; i >= 0 ; i--)
			{
				// Get Calculated AminoAcidDistribution from aminoAcidDistributuion.js
				let AcidMass = getAminoAcidDistribution(sequence[i])[0].mass;
				if(i == temp_seqln)
				{
					// Add 1 to i as the seq start from 0 but the peak id starts from 1
					let position = temp_seqln - i;
					// Adding water mass for suffix, indirectly this will add water mass to all the masses
					let mass = AcidMass;//
					if(suffixMassList != 0)
					{
						mass = this.addMassShift(i,massShiftList,mass)
					}
					let tempObj = {position:(position+1),mass:mass} ;
					suffixMassList[position] = tempObj;
				}
				else
				{
					//Don't add water here
					let position = temp_seqln - i;
					let mass = suffixMassList[(position-1)].mass + AcidMass;
					if(suffixMassList != 0)
					{
						mass = this.addMassShift(i,massShiftList,mass)
					}
					// Add 1 to i as the seq start from 0 but the peak id starts from 1
					let tempObj = {position:(position+1),mass:mass};
					suffixMassList[position] = tempObj;
				}
			}
			// let utilFunctionsObj = new utilFunctions();
			// let ionMassShift = utilFunctionsObj.getCTerminusMassShiftVal();
			for(let j=0;j<seqln;j++)
			{
				suffixMassList[j].mass = suffixMassList[j].mass + massShift_in  ;
			}
			return suffixMassList;
		}
		return emptyMassList ;
	}
	this.getTotalSeqMass = function(seq,massShiftList){
		let mass = 0 ;
		let len = seq.length;
		for(let i=0;i<len;i++)
		{
			mass = mass + getAminoAcidDistribution(seq[i])[0].mass;
		}
		let shiftlen = massShiftList.length;
		for(let j=0;j<shiftlen;j++)
		{
			mass = mass + massShiftList[j].mass;
		}
		mass = mass + this.addWaterMass();
		return mass ;
	}
	// Function to add mass shift
	this.addMassShift = function(position,massShiftList,mass){
		let len = massShiftList.length;
		for(let i=0;i<len ; i++)
		{
			if(position == massShiftList[i].position)
			{
				mass = mass + massShiftList[i].mass ;
				return mass ;
			}
		}
		return mass ;
	}
	// Function to add water to SuffixMass List
	this.addWaterMass = function(){
		mass = getAminoAcidDistribution(WATER)[0].mass ;
		return mass ;
	}
}
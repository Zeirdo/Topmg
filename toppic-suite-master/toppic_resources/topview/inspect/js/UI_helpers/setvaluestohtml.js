class SetValuesToHtml{
    constructor(peakAndIntensityList,massAndIntensityList,sequence,l_fixedPtmList){
        this.peakAndIntensityList = peakAndIntensityList;
        this.massAndIntensityList = massAndIntensityList;
        this.sequence = sequence;
        this.fixedPtmList = l_fixedPtmList;
    }
    setDataToPeakAndIntensity(){
        let peakdata = $("#peakdata");
        peakdata.val(this.peakAndIntensityList[0]);
        for(let i=1;i<this.peakAndIntensityList.length;i++)
        {
            peakdata.val( peakdata.val() + "\n" + this.peakAndIntensityList[i]);
        }
    }
    setDataToMassAndIntensity(){
        let massdata = $("#data");
        massdata.val(this.massAndIntensityList[0]);
        for(let i=1;i<this.massAndIntensityList.length;i++)
        {
            massdata.val( massdata.val()+"\n"+this.massAndIntensityList[i]);
        }
    }
    setDataToSequence(){
        $("#sequencedata").val(this.sequence);
    }
    setFixedMasses(){
        if(this.fixedPtmList.length !=0)
        {
            let commonFixedPtmsObj = new commonFixedPtms();
            let comonfixedPtmList = commonFixedPtmsObj.fixedPtmList; 
            for(let i=0;i<this.fixedPtmList.length;i++)
            {
                for(let j=0; j<comonfixedPtmList.length;j++)
                {
                    if(this.fixedPtmList[i].name.toUpperCase() ==  comonfixedPtmList[j].name.toUpperCase())
                    {
                        let fixedptm = comonfixedPtmList[j].acid + ":" + comonfixedPtmList[j].mass;
                        commonFixedPtms.addNewFixedPtmRow(fixedptm);
                        break;
                    }
                }
            }
        }
        
    }
    
}
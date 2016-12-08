import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}.{ext}"
t_fit_config_file = "fit_{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "DcaMap_{plc}_{{jobIndex}}.{ext}"
t_fit_product_file = "DcaMap.{ext}"



def write_conf( data_path, output_path, input_config_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>DcaMapMaker</jobType>
	<Task name="DcaMapMaker" type="DcaMapMaker" config="" nodePath="DcaMapMaker" />

	<DcaMapMaker tofMinPt="{tofMinPt}" >
		<Logger color="true" logLevel="all" globalLogLevel="info" />
		
		<input plc="{plc}">
			<dst treeName="SpectraPicoDst" url="{data_path}" splitBy="40"/>
		</input>

		<output path="{product_path}">
			<TFile url="{product_path}/{product_file}" />
			<Reports>report/</Reports>
		</output>

		<Bichsel>
			<table>dedxBichsel.root</table>
			<method>0</method>
		</Bichsel>
		
		<ZRecentering>
			<centerSpecies>{plc}</centerSpecies>
			<sigma tof="0.012" dedx="0.07"/>
			<method>linear</method>
		</ZRecentering>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />
		<MakeQA event="true" track="false" tof="false" dedx="false"/>

		<histograms>
			<Histo name="pt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt"/>
			<Histo name="dca_vs_pt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt" bins_y="binning.dca"/>
			<Histo name="corrPt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt" bins_y="binning.eloss"/>
		</histograms>

		<!-- Energy Loss params are loaded from their own Configs -->
		<EnergyLossParams path="{params_path}" />

		<!-- InclusiveSpectra can skip making the combined spectra -->
		<Spectra all="false" tof="false" />

	</DcaMapMaker>
	

	<!-- Include common resources -->
	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />
	<Include url="../common/qaHistograms.xml" />

</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	minTofPtMap = {
		"Pi" : 0.6,
		"K" : 0.6,
		"P" : 0.8
	}
	# data files must have the name form '{Plc}'

	for plc in plcs :
		product_file = t_product_file.format( plc=plc, ext="root" )
		
		#energy loss params
		param_file= pjoin( input_config_path, "EnergyLoss_" )


		tofMinPt = minTofPtMap[ plc ]
		with open( pjoin( config_path, t_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, data_path=data_path, params_path=param_file, product_path=output_path, product_file=product_file, tofMinPt=tofMinPt ) )

def write( data_path, output_path, input_config_path, config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)

	write_conf( data_path, output_path, input_config_path, config_path )
	




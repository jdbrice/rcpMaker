import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}.{ext}"
t_fit_config_file = "fit_{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "ZdData_{plc}_{{jobIndex}}.{ext}"
t_fit_product_file = "ZdYield_{plc}.{ext}"



def write_conf( data_path, output_path, input_config_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>ZdDataMaker</jobType>
	<Task name="ZdDataMaker" type="ZdDataMaker" config="" nodePath="ZdDataMaker" />

	<ZdDataMaker trackBytrackCorrs="{tbtCorrs}" xVar="mTm0">
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
			<Histo name="zd" title="zd; zd; dN/dzd"  bins_x="binning.dedx"/>
			<Histo name="pt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt"/>
			<Histo name="corrPt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt" bins_y="binning.eloss"/>
		</histograms>

		<!-- Energy Loss params are loaded from their own Configs -->
		<EnergyLossParams path="{params_path}" />

		<!-- InclusiveSpectra can skip making the combined spectra -->
		<Spectra all="false" tof="false" />

		<!-- Include the corrections files -->
		<!-- systematics = nSigma ==> nSigma to vary for systematics : default is 0 -->
		<TpcEff systematics="0" >
			<Include url="../Params/TpcEff.xml" />
		</TpcEff>

	</ZdDataMaker>
	

	<!-- Include common resources -->
	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />
	<Include url="../common/qaHistograms.xml" />

</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	# data files must have the name form '{Plc}'

	for plc in plcs :
		product_file = t_product_file.format( plc=plc, ext="root" )
		
		#energy loss params
		param_file= pjoin( input_config_path, "EnergyLoss_" )

		with open( pjoin( config_path, t_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, data_path=data_path, params_path=param_file, product_path=output_path, product_file=product_file, tbtCorrs="true" ) )

def write_fit_conf( output_path, config_path ):
	template = """<?xml version="1.0" encoding="UTF-8"?>
<config>

	<Task name="ZdSpectraHistoFileMaker" type="ZdSpectraHistoFileMaker" config="" nodePath="ZDSHFM" />

	<ZDSHFM>
		<Logger color="true" globalLogLevel="info" />
		
		<input plc="{plc}">
			<TFile url="{product_path}ZdData_{{ZDSHFM.input:plc}}.root" name="spectra" />
		</input>

		<output>
			<TFile url="{product_path}ZdYield_{{ZDSHFM.input:plc}}.root" />
		</output>

		<histograms>
			<Histo name="pT" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt"/>
		</histograms>

		<Include url="../common/centralityMap.xml" />

	</ZDSHFM>

	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />

</config>
	"""

	plcs 		= ( "Pi", "K", "P" )
	for plc in plcs :
		with open( pjoin( config_path, t_fit_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, product_path=output_path ) )

def write( data_path, output_path, input_config_path, config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)

	write_conf( data_path, output_path, input_config_path, config_path )
	write_fit_conf( output_path, config_path )






# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
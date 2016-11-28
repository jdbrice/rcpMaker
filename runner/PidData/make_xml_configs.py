import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{state}_{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "PidData_{state}_{plc}.{ext}"



def write_conf( data_path, output_path, input_config_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>PidDataMaker</jobType>
	<Task name="PidDataMaker" type="PidDataMaker" config="" nodePath="PidDataMaker" />

	<PidDataMaker trackBytrackCorrs="{tbtCorrs}">
		<Logger color="true" logLevel="all" globalLogLevel="info" />
		
		<input plc="{plc}">
			<dst treeName="SpectraPicoDst" url="{data_path}" splitBy="40"/>
		</input>

		<output path="{product_path}">
			<data>{product_file}</data>
			<Reports>report/</Reports>
		</output>

		<Bichsel>
			<table>dedxBichsel.root</table>
			<method>0</method>
		</Bichsel>
		
		<ZRecentering>
			<centerSpecies>{plc}</centerSpecies>
			<sigma tof="0.012" dedx="0.07"/>
			<method>nonlinear</method>
		</ZRecentering>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />
		<MakeQA event="true" track="false" tof="false" dedx="false"/>

		<histograms>
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

	</PidDataMaker>
	

	<!-- Include common resources -->
	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />
	<Include url="../common/qaHistograms.xml" />

</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	# data files must have the name form '{Plc}'

	state = "PostCorr"	
	for plc in plcs :
		product_file = t_product_file.format( state=state, plc=plc, ext="root" )
		
		#energy loss params
		param_file= pjoin( input_config_path, "EnergyLoss_" )

		with open( pjoin( config_path, t_config_file.format( state=state, plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, data_path=data_path, params_path=param_file, product_path=output_path, product_file=product_file, tbtCorrs="true" ) )


def write( data_path, output_path, input_config_path, config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)

	write_conf( data_path, output_path, input_config_path, config_path )


# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
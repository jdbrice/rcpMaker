import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}_{c}.{ext}"
t_data_file = "{plc}_{c}_{tt}.{ext}"
# all of them should define this
t_product_file = "EnergyLoss_{plc}_{c}.{ext}"



def write_conf( data_path, output_path, output_config_path, config_path ="./" ) :
	template = """
	<?xml version="1.0" encoding="UTF-8"?>
	<config>

		<!-- Job to run -->
		<jobType>EnergyLoss</jobType>

		<EnergyLoss>
			<Logger color="false" globalLogLevel="info" logLevel="all" />
			
			<input plc="{plc}">
				<dst treeName="rcpPicoDst" url="{data_file}"/>
			</input>
			<output>
				<data>{product_file}</data>
				<params>{params_file}</params>
			</output>

			<!-- the bins into which the 9 centrality bins are mapped. -->
			<Include url="../Common/centralityMap.xml" />

			<MakeQA event="false" track="false" />

			<!-- histograms to auto-make -->
			<histograms>
				<Histo name="pt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  xBins="binning.pt"/>
				<Histo name="energyLoss" title="Energy Loss; RC p_{{T}} [GeV]; p_{{T}}^{{RC}} - p_{{T}}^{{MC}}"  xBins="binning.pt" yBins="binning.eloss"/>
			</histograms>

			<!-- InclusiveSpectra can skip making the combined spectra -->
			<Spectra all="false" tof="false" />

		</EnergyLoss>

		<!-- Include common resources -->
		<Include url="../Common/cuts.xml" />
		<Include url="../Common/binning.xml" />
		<Include url="../Common/qaHistograms.xml" />


	</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	charge 		= ( "p", "n" )
	# data files must have the name form '{Plc}_{charge}'


	for plc in plcs :
		for c in charge :
				data_file = pjoin( data_path, t_data_file.format( plc=plc, c=c, tt="rc", ext="root" ) )
				prod_file = pjoin( output_path, t_product_file.format( plc=plc, c=c, ext="root" ) )
				param_file= pjoin( output_config_path, t_product_file.format( plc=plc, c=c, ext="xml" ) )

				with open( pjoin( config_path, t_config_file.format( plc=plc, c=c, ext="xml" ) ), 'w' ) as f :
					f.write( template.format( plc=plc, data_file=data_file, params_file=param_file, product_file=prod_file ) )


def write( data_path, output_path, output_config_path, config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)

	write_conf( data_path, output_path, output_config_path, config_path )


# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
import os
import PidHisto.make_xml_configs as pidc
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "Fit_{plc}.{ext}"



def write_conf(  output_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>SimultaneousTPid</jobType>

	<SimultaneousPid>
		<Logger color="true" logLevel="info" globalLogLevel="info"/>
		<Reporter> <output width="800" height="800" /> </Reporter>	

		<!-- Path the PidHisto file -->
		<input> <data url="{data_file}"/> </input>
		<output path="{output_path}"> <data>{product_file}</data> </output>
	

		<!-- fit bins -->
		<FitRange>
			<ptBins min="0" max="30" />
			<centralityBins>0, 1, 2, 3, 4, 5, 6</centralityBins>
			<charges>1, -1</charges>
		</FitRange>


		<!-- Guiding parameters -->
		<Timing zdOnly="0.5" useZdEnhanced="0.5" useZbEnhanced="0.5" nSigZdEnhanced="3.0" nSigZbEnhanced="3.0" />

		<ParameterFixing>
			<!-- mu can vary +/- nSigma -->
			<deltaMu zb="6.0" zd="13.0" />

			<!-- the minimum momentum at which the value is fixed -->
			<Pi zbSigma="0.65" zdSigma="1.915" />
			<K zbSigma="0.65" zdSigma="1.915" />
			<P zbSigma="1.33" zdSigma="1.915" />

		</ParameterFixing>
		

		<Bichsel>
			<table>dedxBichsel.root</table>
			<method>0</method>
		</Bichsel>
		
		<ZRecentering>
			<centerSpecies>{plc}</centerSpecies>
			<sigma tof="0.012" dedx="0.07"/>
			<method>nonlinear</method>
		</ZRecentering>

		<Include url="../common/fitSchema.xml" />

		<histograms>
			<Histo name="yield" title="yield" xBins="binning.pt" />
		</histograms>

	</SimultaneousPid>


	<Include url="../common/binning.xml" />

</config>

	"""


	plcs 		= ( "Pi", "K", "P" )
	for plc in plcs :
		data_file = pjoin( output_path, pidc.t_product_file.format( plc=plc, ext="root" ) )

		with open( pjoin( config_path, t_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, data_file=data_file, output_path=output_path, product_file=t_product_file.format( plc=plc, ext="root" ) ) )


def write( output_path, config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)

	write_conf( output_path, config_path )


# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
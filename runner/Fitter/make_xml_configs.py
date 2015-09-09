import os
import PidData.make_xml_configs as pidc
pjoin = os.path.join


# all of them should define this
t_config_file = "{state}_{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "Fit_{state}_{plc}.{ext}"



def write_conf(  output_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>SimultaneousTPid</jobType>

	<SimultaneousPid>
		<Logger color="true" logLevel="info" globalLogLevel="info"/>
		<Reporter> <output width="1600" height="1200" /> </Reporter>	

		<!-- Path the PidHisto file -->
		<input> <data url="{data_file}"/> </input>
		<output path="{output_path}"> <data>{product_file}</data> </output>
	
		<!-- Systematic Uncertainties -->
		<Systematics sigma="false" tofEff="false" tofEffAmount="0.15" nTofEff="10" nSigma="5" />

		<!-- fit bins -->
		<FitRange>
			<ptBins min="10" max="32" />
			<centralityBins>0, 1, 2, 3, 4, 5, 6</centralityBins>
			<charges>1, -1</charges>
		</FitRange>


		<!-- Guiding parameters -->
		<Timing zdOnly="0.2" useZdEnhanced="0.2" useZbEnhanced="0.2" nSigZdEnhanced="3.0" nSigZbEnhanced="6.0" />

		<ParameterFixing>
			<!-- mu can vary +/- nSigma -->
			<deltaMu zb="2.0" zd="2.0" />

			<!-- 
				The ranges for width determination and fixing. The value if free to float below max. It's mean is taken between min and max and used as the value above max. The std deviation in this range is used as the systematic unct.
			-->
			<zb>
				<Pi min="0.6" max="1.0"/>
				<K min="0.6" max="1.0" />
				<P min="1.0" max="1.4" />
			</zb>
			<zd>
				<Pi min="0.6" max="1.0"/>
				<K min="0.6" max="1.0" />
				<P min="0.6" max="1.0" />
			</zd>

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

		<DataPurity nSigmaPi="3.0" nSigmaK="3.0" nSigmaE="3.0" nSigmaAboveP="3.0" />

		<Include url="../common/fitSchema_eff.xml" />

		<histograms>
			<Histo name="yield" title="yield" xBins="binning.pt" />
			<Histo name="eff_dist" title="eff_dist" xBins="binning.pt" widthY=".01" minY="0.75" maxY="1.25" />
			<Histo name="sigma_dist" title="delta sigma_dist" xBins="binning.pt" widthY=".0001" minY="-0.1" maxY="0.1" />
			<Histo name="sys_dist" title="eff_dist" xBins="binning.pt" widthY=".005" minY="-1.0" maxY="1.0" />
		</histograms>

	</SimultaneousPid>


	<Include url="../common/binning.xml" />

</config>

	"""

	states 		= ( "Corr", "PostCorr" )
	plcs 		= ( "Pi", "K", "P" )
	for state in states :
		for plc in plcs :
			data_file = pjoin( output_path, pidc.t_product_file.format( state=state, plc=plc, ext="root" ) )

			with open( pjoin( config_path, t_config_file.format( state=state, plc=plc, ext="xml" ) ), 'w' ) as f :
				f.write( template.format( plc=plc, data_file=data_file, output_path=output_path, product_file=t_product_file.format( state=state, plc=plc, ext="root" ) ) )


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
import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{state}_{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "PidHisto_{state}_{plc}.{ext}"



def write_conf( data_path, output_path, input_config_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>PidHistoMaker</jobType>

	<PidHistoMaker>
		<Logger color="true" logLevel="all" globalLogLevel="info" />
		
		<input plc="{plc}">
			<dst treeName="rcpPicoDst" url="{data_path}"/>
		</input>

		<output path="{product_path}">
			<data>{product_file}</data>
			<Reports>report/</Reports>
		</output>

		<Bichsel>
			<table>dedxBichsel.root</table>
			<method>0</method>
		</Bichsel>

		<Distributions 2D="false" 1D="true" enhanced="true" tof="1.0" dedx="1.0" nSigBelow="3.0" nSigAbove="3.0"/>
		<Electrons nSigE="3" nSigPi="3" nSigK="3" nSigP="6" />
		
		<ZRecentering>
			<centerSpecies>{plc}</centerSpecies>
			<sigma tof="0.012" dedx="0.07"/>
			<method>nonlinear</method>
		</ZRecentering>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />
		<MakeQA event="true" track="false" tof="false" dedx="false"/>

		<histograms>
			<Histo name="betaRaw" title="#beta^{{-1}}" xBins="binning.rid" yBins="binning.tofRaw" />
			<Histo name="dedxRaw" title="dEdx" xBins="binning.rid" yBins="binning.dedxRaw" />

			<Histo name="cutBeta" title="#beta^{{-1}}" xBins="binning.rid" yBins="binning.tofRaw" />
			<Histo name="cutDedx" title="dE/dx" xBins="binning.rid" yBins="binning.dedxRaw" />

			<Histo name="eta" type="1D" title="eta" nBinsX="200" minX="-2" maxX="2" />
			<Histo name="pt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  xBins="binning.pt"/>

			<Histo name="corrPt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  xBins="binning.pt" yBins="binning.eloss"/>
			

			<Histo name="trBeta" title="" xBins="binning.pt" yBins="binning.tof" />
			<Histo name="trDedx" title="" xBins="binning.pt" yBins="binning.dedx" />
			<Histo name="nlBeta" title="" xBins="binning.pt" yBins="binning.tof"/>
			<Histo name="nlDedx" title="" xBins="binning.pt" yBins="binning.dedx"/>
		</histograms>

		<style hide="true">
			<tof title="#beta^{{-1}}" logy="1" draw="pe"/>
			<dedx title="dedx" logy="1" draw="pe"/>
		</style>
		

		<!-- Energy Loss params are loaded from their own Configs -->
		<EnergyLossParams path="{params_path}" />

		<!-- InclusiveSpectra can skip making the combined spectra -->
		<Spectra all="false" tof="false" />

		<Corrections tof="{corr}" tpc="{corr}" fd="{corr}"/>
		<!-- Include the corrections files -->
		<FeedDown>
			<Include url="../Params/FeedDown.xml" />
		</FeedDown>
		<TpcEff>
			<Include url="../Params/TpcEff.xml" />
		</TpcEff>
		<TofEff>
			<Include url="../Params/TofEff.xml" />
		</TofEff>

	</PidHistoMaker>
	

	<!-- Include common resources -->
	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />
	<Include url="../common/qaHistograms.xml" />

</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	states 		= ("Corr", "PostCorr")
	# data files must have the name form '{Plc}'

	for state in states :
		for plc in plcs :
			product_file = t_product_file.format( state=state, plc=plc, ext="root" )
			corr = 'false'
			if "Corr" == state :
				corr = 'true'

			#energy loss params
			param_file= pjoin( input_config_path, "EnergyLoss_" )

			with open( pjoin( config_path, t_config_file.format( state=state, plc=plc, ext="xml" ) ), 'w' ) as f :
				f.write( template.format( plc=plc, corr=corr, data_path=data_path, params_path=param_file, product_path=output_path, product_file=product_file ) )


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
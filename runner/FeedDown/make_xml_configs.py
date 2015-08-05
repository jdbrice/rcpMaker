import os
pjoin = os.path.join


# all of them should define this
t_config_file = "feedDown.{ext}"
# all of them should define this
t_product_file = "FeedDown.{ext}"

def write_conf( data_path, output_path, output_config_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>
	
	<jobType>FeedDownMaker</jobType>

	<FeedDownMaker>
		
		<SkipMake>false</SkipMake>
		<Pages parents="true" fraction="true" export="false"/>

		<!-- Use a datasource to setup an automap to the tree structure -->
		<DataSource treeName="StMiniMcTree" url="{data_path}">
			
			<!-- Set all branches off -->
			<BranchStatus status="0">*</BranchStatus>
			<!-- Turn on only the ones you need -->
			<BranchStatus status="1">
				mMatchedPair*,
				mVertex*,
				mNMatchedPair,
				mNUnc*
			</BranchStatus>
			
		</DataSource>

		<Reporter>
			<output url="{report_file}" width="1400" height="700"/>
		</Reporter>
		<Logger logLevel="info" globalLogLevel="warning" />
		<output>
			<data>{product_file}</data>
			<param>{params_file}</param>
		</output>

		<histograms>
			<Histo name="vR" title="vR [cm]" xBins="bins.vR" />
			<Histo name="vX" title="vX [cm]" xBins="bins.vX" />
			<Histo name="vY" title="vY [cm]" xBins="bins.vX" />
			<Histo name="vZ" title="vZ [cm]" xBins="bins.vZ" />
			<Histo name="refMult" title="rm" xBins="bins.rm" />
			<Histo name="refMultBins" title="rm" xBins="bins.rm" yBins="bins.rmb" />
			<Histo name="refMultMappedBins" title="rm" xBins="bins.rm" yBins="bins.rmb" />

			<Histo name="pre_rapidity" title="rapidity" widthX=".01" minX="-2.0" maxX="2.0" />

			<Histo name="dca" title="DcaGl [cm]" widthX="0.02" minX="0.0" maxX="3.0" />
			<Histo name="fitPts" title="fitPts" widthX="1" minX="0.0" maxX="50.0" />
			<Histo name="dedxPts" title="dedxPts" widthX="1" minX="0.0" maxX="50.0" />
			<Histo name="possiblePts" title="possiblePts" widthX="1" minX="0.0" maxX="50.0" />
			<Histo name="fitr" title="fitPts / possiblePts" widthX=".02" minX="0.0" maxX="1.0" />
			<Histo name="ptr" title="ptgl / ptpr" widthX=".01" minX="0.0" maxX="2.0" />
			<Histo name="eta" title="eta" widthX=".02" minX="-2.0" maxX="2.0" />
			<Histo name="rapidity" title="rapidity" widthX=".01" minX="-0.3" maxX="0.3" />

			<Histo name="parents" widthX="1" minX="-0.5" maxX="51.5" yBins="bins.pt" />
			<Histo name="spectra" xBins="bins.pt" />

			<Histo name="refMultVz" xBins="bins.vZ" yBins="bins.rm" />

		</histograms>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />

	</FeedDownMaker>


	<bins>
		<vR width="0.01" min="0.0" max="3.0" />
		<vX width="0.01" min="-1.5" max="1.5" />
		<vZ width="1" min="-30.0" max="30.0" />

		<rm width="1" min="0" max="400" />
		<rmb width="1" min="0" max="10" />

		<pt> 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.5, 4.5 </pt>
	</bins>


	<Style>
		<energyLoss draw="colz" logz="1" optstat="0" xr="0, 2.0" yr="-0.1, 0.1" />
		<energyLoss1D draw="pe same" logz="0" optstat="0" xr="0, 2.0" mst="8" mc="black" ms="1.5" lc="black" lw="3" yr="-0.1, 0.1"/>
		<logy1D draw="" logy="1" optstat="0" fc="red" xr="-0.5, 33.5"/>

		<frac title="Background Fraction" y="Background / Total" logy="1" xr="0.0, 2.5" ms="2" mst="8" xr="0, 2.0" yto="0.5" yts="0.06" optstat="0"/>
		<frac_P_Plus  yr="2e-2, 1" />
		<frac_P_Minus  yr="2e-2, 1" />

		<frac_Pi_Plus  yr="5e-3, 2e-1" />
		<frac_Pi_Minus  yr="5e-3, 2e-1" />

		<ratio title="" y="data / fit"  yls="0.1" xls="0.1" logy="0" yr="0.8, 1.25" xr="0, 2.0" mst="8" mc="black" lc="black" yto="0.2" yts="0.15" />
	</Style>

	<!-- Include common resources -->
	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />
	<Include url="../common/qaHistograms.xml" />

</config>
	"""

	
	report_file = pjoin( output_path, "rp" + t_product_file.format( ext="pdf" ) )
	product_file = pjoin( output_path, t_product_file.format( ext="root" ) )
	params_file= pjoin( output_config_path, t_product_file.format( ext="xml" ) )

	with open( pjoin( config_path, t_config_file.format( ext="xml" ) ), 'w' ) as f :
		f.write( template.format( data_path=data_path, params_file=params_file, product_file=product_file, report_file=report_file ) )


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
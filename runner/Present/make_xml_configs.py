import os
import Fitter.make_xml_configs as fitc
import PidHisto.make_xml_configs as pidc
import PostCorr.make_xml_configs as postc
pjoin = os.path.join


# all of them should define this
t_config_file = "{state}_{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "Present_{state}_{plc}.{ext}"



def write_conf(  output_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>PresentPidYield</jobType>

	<PidYieldPresenter>
		<Logger color="true" logLevel="info" globalLogLevel="warning" />
		<Reporter> <output url="{report_file}" width="800" height="500" /> </Reporter>	

		<input plc="{plc}">
			<fit url="{fit_file}" />
			<ps url="{histo_file}" />
			
		</input>

		<output path="{output_path}">
			<data url="{product_file}"/>
		</output>

		<FitRange >
			<ptBins min="0" max="30" />
			<centralityBins>0, 1, 2, 3, 4, 5, 6</centralityBins>
			<charges>1, -1</charges>

		</FitRange>

		<LastYieldBin>
			<Pi p="37" n="37" />
			<K p="37" n="37"/> 
			<P p="37" n="37"/> 
		</LastYieldBin>

		<nColl> 787.915, 633.514, 454.215, 282.678, 167.971, 71.739, 18.3058 </nColl>
		<nPart ptBin="15" > 337.893, 289.13, 225.593, 158.731, 108.064, 56.7823, 19.4839 </nPart>
		<colors>632, 416, 600, 616, 800, 880, 820, 840, 860, 900</colors>


		<centralityLabels>0-5%, 5-10%, 10-20%, 20-30%, 30-40%, 40-60%, 60-80% </centralityLabels>


		<Style>
			<chargeRatio lineWidth="2" lineColor="red" domain="0.5, 3.4"/>
			<chargeRatio_Pi range="0.8, 1.4" y="#pi^{{-}} / #pi^{{+}}" />
			<chargeRatio_K range="0.2, 0.8" y="K^{{-}} / K^{{+}}" />
			<chargeRatio_P  range="0.01, 0.2" y="Pbar / P" />

			<rcp domain="0.0, 7" range=".1, 10" logY="1" numberofTicks="7, 5"/>
			<nPart range=".5, 4" logY="1" linewidth="2" markerStyle="20" markerColor="red" x="<nPart>" y="R_{{CP}}" />
		</Style>

	</PidYieldPresenter>


	<Include url="../common/binning.xml" />

</config>
	"""

	states 		= ( "Corr", "PostCorr" )
	plcs 		= ( "Pi", "K", "P" )
	for state in states :
		for plc in plcs :
			
			if "Corr" == state :
				fit_file = pjoin( output_path, pidc.t_product_file.format( state=state, plc=plc, ext="root" ) )
			else :
				fit_file = pjoin( output_path, postc.t_product_file.format(  plc=plc, ext="root" ) )

			histo_file = pjoin( output_path, pidc.t_product_file.format( state=state, plc=plc, ext="root" ) )
			product_file = t_product_file.format( state=state, plc=plc, ext="root" )
			report_file = pjoin( output_path, "rp" + product_file )

			with open( pjoin( config_path, t_config_file.format( state=state, plc=plc, ext="xml" ) ), 'w' ) as f :
				f.write( template.format( plc=plc, fit_file=fit_file, histo_file=histo_file, output_path=output_path, report_file=report_file, product_file=product_file ) )


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
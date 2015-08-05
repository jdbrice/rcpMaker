import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "TofEff.{ext}"
t_histo_file = "TofEff_{plc}.{ext}"




def write_conf( data_path, output_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<!-- Job to run -->
	<jobType>InclusiveSpectra</jobType>

	<InclusiveSpectra>
		<Logger color="true" globalLogLevel="info" logLevel="all" />
		
		<input plc="{plc}" >
			<dst treeName="rcpPicoDst" url="{data_path}" />
		</input>
		<output>
			<data>{product_file}</data>
		</output>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />

		<MakeQA event="false" track="false" />

		<!-- histograms to auto-make -->
		<histograms>
			<pt type="1D" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  xBins="binning.pt"/>
		</histograms>

	</InclusiveSpectra>

	<!-- Include common resources -->
	<Include url="../common/cuts.xml" />
	<Include url="../common/binning.xml" />
	<Include url="../common/qaHistograms.xml" />


</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	# data files must have the name form '{Plc}'


	for plc in plcs :
		prod_file = pjoin( output_path, t_histo_file.format( plc=plc, ext="root" ) )
		
		with open( pjoin( config_path, t_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, data_path=data_path, product_file=prod_file ) )


def write_fit_config( input_path, output_path, output_config_path, config_path ="./" ) :
	""" Make the single config used to do the division and fitting"""


	fit_template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<!-- Job to run -->
	<jobType>TofEffFitter</jobType>

	<TofEffFitter>
		<Logger color="true" globalLogLevel="info" logLevel="all" />
		<Reporter> <output url="{report_file}" width="1200" height="800" /> </Reporter>

		<input url="{input_path}"/> 

		<output>
			<data>{product_file}</data>
			<params>{params_file}</params>
		</output>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />

	</TofEffFitter>

</config>"""


	report = pjoin( output_path, "rp_" + t_product_file.format( ext="pdf" ) )
	product = pjoin( output_path, t_product_file.format( ext="root" ) )
	params = pjoin( output_config_path, t_product_file.format( ext="xml" ) )

	with open( pjoin( config_path, 'fit.xml' ), 'w' ) as f :
		f.write( fit_template.format( input_path = output_path, params_file = params, product_file=product, report_file=report ) )

def write( data_path, output_path, output_config_path, config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)

	write_conf( data_path, output_path, config_path )
	write_fit_config( data_path, output_path, output_config_path, config_path )


# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
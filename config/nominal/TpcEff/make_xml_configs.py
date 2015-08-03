import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}_{c}_{tt}.{ext}"
# all of them should define this
t_product_file = "TpcEff.root"



def write_histo_conf( data_path, output_path, config_path ="./" ) :
	template = """
	<?xml version="1.0" encoding="UTF-8"?>
	<config>

		<!-- Job to run -->
		<jobType>InclusiveSpectra</jobType>

		<InclusiveSpectra>
			<Logger color="true" globalLogLevel="info" logLevel="all" />
			
			<input plc="{0}">
				<dst treeName="rcpPicoDst" url="{1}"/>
			</input>
			<output>
				<data>{2}</data>
			</output>

			<!-- the bins into which the 9 centrality bins are mapped. -->
			<Include url="../common/centralityMap.xml" />

			<MakeQA event="true" track="true" />

			<!-- histograms to auto-make -->
			<histograms>
				<pt type="1D" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  xBins="binning.ptEff"/>
			</histograms>

		</InclusiveSpectra>

		<!-- Include common resources -->
		<Include url="../common/cuts.xml" />
		<Include url="../common/binning.xml" />
		<Include url="../common/qaHistograms.xml" />

	</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	charge 		= ( "p", "n" )
	track_types = ( "mc", "rc" )
	# data files must have the name form '{Plc}_{charge}'

	# intermediate - not seen from outside
	t_product_histo_file = "TpcEff_" + t_config_file


	for plc in plcs :
		for c in charge :
			for tt in track_types :
				plc_c = plc + "_" + c
				#plc_c +"_" + tt + ".root"
				data_file = pjoin( data_path, t_config_file.format( plc=plc, c=c, tt=tt, ext="root" ) )
				prod_file = pjoin( output_path, t_product_histo_file.format( plc=plc, c=c, tt=tt, ext="root" ) )

				with open( pjoin( config_path, t_config_file.format( plc=plc, c=c, tt=tt, ext="xml" ) ), 'w' ) as f :
					f.write( template.format( plc, data_file, prod_file ) )





def write_fit_config( input_path, output_path, config_path ="./" ) :
	""" Make the single config used to do the division and fitting"""


	fit_template = """
	<?xml version="1.0" encoding="UTF-8"?>
	<config>

		<!-- Job to run -->
		<jobType>TpcEffFitter</jobType>

		<TpcEffFitter>
			<Logger color="true" globalLogLevel="info" logLevel="all" />
			
			<input url="{input_path}"/> 

			<output path="{output_path}">
				<data>{product}</data>
			</output>

			<!-- the bins into which the 9 centrality bins are mapped. -->
			<Include url="../common/centralityMap.xml" />

		</TpcEffFitter>

	</config>"""


	with open( pjoin( config_path, 'fit.xml' ), 'w' ) as f :
		f.write( fit_template.format( input_path = input_path, output_path = output_path, product=t_product_file ) )

def write( data_path, output_path, config_path ="./" ) :

	write_histo_conf( data_path, output_path, config_path )
	write_fit_config( output_path, output_path, config_path )


# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
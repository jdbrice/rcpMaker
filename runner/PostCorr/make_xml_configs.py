import os
import Fitter.make_xml_configs as fitc
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}.{ext}"
t_data_file = "{plc}.{ext}"
# all of them should define this
t_product_file = "PostCorr_{plc}.{ext}"



def write_conf(  output_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>ApplyPostCorr</jobType>

	<ApplyPostCorr>
		<Logger color="true" logLevel="info" globalLogLevel="trace"/>

		<input plc="{plc}">
			<data url="{data_file}" />
		</input>
		<output>
			<data>{product_file}</data>
		</output>


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

		<!-- Include the centrality maps -->
		<Include url="../common/CentralityMap.xml" />

	</ApplyPostCorr>

</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	for plc in plcs :
		data_file = pjoin( output_path, fitc.t_product_file.format( plc=plc, ext="root" ) )
		product_file = pjoin( output_path, t_product_file.format( plc=plc, ext="root" ) )

		with open( pjoin( config_path, t_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, data_file=data_file,  product_file=product_file ) )


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
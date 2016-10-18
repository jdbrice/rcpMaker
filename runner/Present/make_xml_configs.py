import os
import Fitter.make_xml_configs as fitc
import PidHisto.make_xml_configs as pidc
import PostCorr.make_xml_configs as postc
pjoin = os.path.join


# all of them should define this
t_config_file = "Export_{plc}.{ext}"

def write_conf(  output_path, config_path ="./" ) :
	template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<jobType>YieldExporter</jobType>
	<Task name="YieldExporter" type="YieldExporter" config="" nodePath="YieldExporter" />

	<Logger color="true" globalLogLevel="info" />

	<YieldExporter>
		<input plc="{plc}" energy="{energy}">
			<data url="{infile}"/>
		</input>

		<output path="{output_path}"/>

		<Include url="../common/centralityMap.xml" />
		<systematics>
			<Include url="../common/systematicUncertainties.xml" />
		</systematics>
	</YieldExporter>
</config>
	"""

	plcs 		= ( "Pi", "K", "P" )
	
	for plc in plcs :
		
		infile = pjoin( output_path, postc.t_product_file.format( plc=plc, ext="root" ) )
	
		with open( pjoin( config_path, t_config_file.format( plc=plc, ext="xml" ) ), 'w' ) as f :
			f.write( template.format( plc=plc, energy="14.5", infile=infile, output_path=output_path  ) )


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
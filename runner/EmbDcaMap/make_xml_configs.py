import os
pjoin = os.path.join


# all of them should define this
t_config_file = "{plc}_{c}_{tt}.{ext}"
# all of them should define this
t_product_file = "DcaWeightMap.{ext}"



def write_histo_conf( data_path, output_path, config_path ="./" ) :
	

	template_mc_rc = """
	<?xml version="1.0" encoding="UTF-8"?>
	<config>

		<!-- Job to run -->
		<jobType>EmbDcaMapMaker</jobType>
		<Task name="EmbDcaMapMaker" type="EmbDcaMapMaker" config="" nodePath="EmbDcaMapMaker" />

		<EmbDcaMapMaker>
			<Logger color="true" globalLogLevel="info" logLevel="all" />
			
			<input plc="{0}" type="{3}" charge="{4}">
				<dst treeName="SpectraPicoDst" url="{1}"/>
			</input>
			<output>
				<TFile url="{2}" />
			</output>

			<!-- the bins into which the 9 centrality bins are mapped. -->
			<Include url="../common/centralityMap.xml" />

			<MakeQA event="true" track="true" />

			<!-- histograms to auto-make -->
			<histograms>
				<Histo name="dca_vs_pt" title="p_{{T}} [GeV]; p_{{T}} [GeV]; #Events"  bins_x="binning.pt" bins_y="binning.dca"/>
			</histograms>

		</EmbDcaMapMaker>

		<!-- Include common resources -->
		<Include url="../common/cuts.xml" />
		<Include url="../common/binning.xml" />
		<Include url="../common/qaHistograms.xml" />

	</config>
	"""


	plcs 		= ( "Pi", "K", "P" )
	charge 		= ( "p", "n" )
	# track_types = ( "rc" )
	tt = "rc"
	# data files must have the name form '{Plc}_{charge}'

	# intermediate - not seen from outside
	t_product_histo_file = "EmbDcaMap_" + t_config_file


	for plc in plcs :
		for c in charge :
			plc_c = plc + "_" + c
			#plc_c +"_" + tt + ".root"
			data_file = pjoin( data_path, t_config_file.format( plc=plc, c=c, tt=tt, ext="root" ) )
			prod_file = pjoin( output_path, t_product_histo_file.format( plc=plc, c=c, tt=tt, ext="root" ) )

			with open( pjoin( config_path, t_config_file.format( plc=plc, c=c, tt=tt, ext="xml" ) ), 'w' ) as f :
				f.write( template_mc_rc.format( plc, data_file, prod_file, tt, c ) )







def write_fit_config( input_path, output_path, output_config_path,config_path ="./" ) :
	""" Make the single config used to do the division and fitting"""


	fit_template = """
<?xml version="1.0" encoding="UTF-8"?>
<config>

	<!-- Job to run -->
	<jobType>DcaWeightMaker</jobType>
	<Task name="DcaWeightMaker" type="DcaWeightMaker" config="" nodePath="DcaWeightMaker" />

	<DcaWeightMaker>
		<Logger color="true" globalLogLevel="info" logLevel="all" />
		<Reporter> 
			<output url="{report_file}" width="700" height="500" /> 
		</Reporter>

		<input>
			<TFile url="{input_file}" />
		</input> 

		<output>
			<TFile url="{output_path}/{product_file}" />
		</output>

		<!-- the bins into which the 9 centrality bins are mapped. -->
		<Include url="../common/centralityMap.xml" />

		<histograms>
			<Histo name="dcaEff" title="; p_{T} [GeV]; DCA<1.0 [cm] %Eff"  bins_x="binning.pt"/>
		</histograms>

	</DcaWeightMaker>
	<Include url="../common/binning.xml" />

</config>"""


	report = pjoin( output_path, "rp_" + t_product_file.format( ext="pdf" ) )
	product = t_product_file.format( ext="root" )
	params = pjoin( output_config_path, t_product_file.format( ext="xml" ) )

	with open( pjoin( config_path, 'fit.xml' ), 'w' ) as f :
		f.write( fit_template.format( input_file = pjoin(output_path, "DcaMap.root"), output_path = output_path, params_file = params, product_file=product, report_file=report ) )

def write( data_path, output_path, output_config_path,config_path ="./" ) :

	if not os.path.exists(config_path):
		os.makedirs(config_path)
	write_histo_conf( data_path, output_path, config_path )
	write_fit_config( output_path, output_path, output_config_path, config_path )


# if __name__ == "__main__":
# 	parser = argparse.ArgumentParser( description="Creates the configs for TpcEff Tasks" );
# 	parser.add_argument( "data_path", help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
# 	parser.add_argument( "output_path", help="Path to folder to output products" )

# 	args = parser.parse_args()

# 	write_histo_conf( args.data_path, args.output_path )
# 	# fitter reads data from and produeces to the output path
# 	write_fit_config( args.output_path, args.output_path )
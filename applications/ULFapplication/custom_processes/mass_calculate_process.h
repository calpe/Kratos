/*
==============================================================================
KratosULFApplication 
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi, Pawel Ryzhakov
pooyan@cimne.upc.edu 
rrossi@cimne.upc.edu
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain


Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/
 
 
//   
//   Project Name:        Kratos       
//   Last Modified by:    $Author: rrossi $
//   Date:                $Date: 2007-05-16 13:59:01 $
//   Revision:            $Revision: 1.4 $ 12 November 2007 - 3D added
//
//		
// THIS PROCESS is INVENTED in order to CALCULATE THE MASS and
//STORE it node-wise. 
//this is necessary for the projection step of ulf-frac method

#if !defined(KRATOS_MASS_CALCULATE_PROCESS_INCLUDED )
#define  KRATOS_MASS_CALCULATE_PROCESS_INCLUDED



// System includes
#include <string>
#include <iostream> 
#include <algorithm>

// External includes 


// Project includes
#include "includes/define.h"
#include "processes/process.h"
#include "includes/node.h"
#include "includes/element.h"
#include "includes/model_part.h"
#include "utilities/geometry_utilities.h"
#include "ULF_application.h"


namespace Kratos
{

	///@name Kratos Globals
	///@{ 

	///@} 
	///@name Type Definitions
	///@{ 


	///@} 
	///@name  Enum's
	///@{

	///@}
	///@name  Functions 
	///@{

	///@}
	///@name Kratos Classes
	///@{

	/// Short class definition.
	/** Detail class definition.
		Update the MASS_FORCE on the nodes

		
	*/

	class MassCalculateProcess 
		: public Process
	{
	public:
		///@name Type Definitions
		///@{

		/// Pointer definition of MassCalculateProcess
		KRATOS_CLASS_POINTER_DEFINITION(MassCalculateProcess);

		///@}
		///@name Life Cycle 
		///@{ 

		/// Default constructor.
		MassCalculateProcess(ModelPart& model_part)
			: mr_model_part(model_part)
		{
		}

		/// Destructor.
		virtual ~MassCalculateProcess()
		{
		}


		///@}
		///@name Operators 
		///@{

		void operator()()
		{
			Execute();
		}


		///@}
		///@name Operations
		///@{

		virtual void Execute()
		{
			KRATOS_TRY
					
			ProcessInfo& proc_info = mr_model_part.GetProcessInfo();
			double dummy;
			//first initialize the Mass force to the old value

			//set the Mass to the old value
			for(ModelPart::NodesContainerType::iterator in = mr_model_part.NodesBegin() ; 
				in != mr_model_part.NodesEnd() ; ++in)
			{  
				in->FastGetSolutionStepValue(NODAL_MASS)=0.0;
			}
			
			for(ModelPart::ElementsContainerType::iterator im = mr_model_part.ElementsBegin() ; 
					im != mr_model_part.ElementsEnd() ; ++im)
			{  
				im->Calculate(NODAL_MASS,dummy,proc_info);
			}
			
			KRATOS_WATCH("Execute of Mass Calulate Process");
	
			
							
		KRATOS_CATCH("")
		}


		///@}
		///@name Access
		///@{ 


		///@}
		///@name Inquiry
		///@{


		///@}      
		///@name Input and output
		///@{

		/// Turn back information as a string.
		virtual std::string Info() const
		{
			return "MassCalculateProcess";
		}

		/// Print information about this object.
		virtual void PrintInfo(std::ostream& rOStream) const
		{
			rOStream << "MassCalculateProcess";
		}

		/// Print object's data.
		virtual void PrintData(std::ostream& rOStream) const
		{
		}


		///@}      
		///@name Friends
		///@{


		///@}

	protected:
		///@name Protected static Member Variables 
		///@{ 


		///@} 
		///@name Protected member Variables 
		///@{ 


		///@} 
		///@name Protected Operators
		///@{ 


		///@} 
		///@name Protected Operations
		///@{ 


		///@} 
		///@name Protected  Access 
		///@{ 


		///@}      
		///@name Protected Inquiry 
		///@{ 


		///@}    
		///@name Protected LifeCycle 
		///@{ 


		///@}

	private:
		///@name Static Member Variables 
		///@{ 


		///@} 
		///@name Member Variables 
		///@{ 
		ModelPart& mr_model_part;
		

		///@} 
		///@name Private Operators
		///@{ 
		
	
		///@} 
		///@name Private Operations
		///@{ 


		///@} 
		///@name Private  Access 
		///@{ 


		///@}    
		///@name Private Inquiry 
		///@{ 


		///@}    
		///@name Un accessible methods 
		///@{ 

		/// Assignment operator.
//		MassCalculateProcess& operator=(MassCalculateProcess const& rOther);

		/// Copy constructor.
//		MassCalculateProcess(MassCalculateProcess const& rOther);


		///@}    

	}; // Class MassCalculateProcess 

	///@} 

	///@name Type Definitions       
	///@{ 


	///@} 
	///@name Input and output 
	///@{ 


	/// input stream function
	inline std::istream& operator >> (std::istream& rIStream, 
		MassCalculateProcess& rThis);

	/// output stream function
	inline std::ostream& operator << (std::ostream& rOStream, 
		const MassCalculateProcess& rThis)
	{
		rThis.PrintInfo(rOStream);
		rOStream << std::endl;
		rThis.PrintData(rOStream);

		return rOStream;
	}
	///@} 


}  // namespace Kratos.

#endif // KRATOS_MASS_CALCULATE_PROCESS_INCLUDED  defined 


